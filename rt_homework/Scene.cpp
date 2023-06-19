#include "Scene.h"
#include "Common.h"
#include "JsonParser.h"
#include "Ray.h"

using namespace std;

Scene::Scene(int imageWidth, int imageHeight)
{
    _settings.ImageWidth = imageWidth;
    _settings.ImageHeight = imageHeight;
}

void Scene::cleanup()
{
    _goemetryObjects.clear();
    _triangles.clear();
    _lights.clear();
    _sceneFile.clear();
    _materials.clear();
    _aabbox.reset();
}

bool Scene::loadScene(const std::string filename)
{
    cleanup();

    JsonParser file(filename);
    if(file.load(*this)) {
        _sceneFile = filename;
        compileGeometry();
        return true;
    }

    return false;
    
}
void Scene::addMesh(Mesh&& mesh)
{
    _goemetryObjects.emplace_back(std::forward<Mesh>(mesh));
}

void Scene::addLight(Light&& light)
{
    _lights.emplace_back(std::forward<Light>(light));
}

void Scene::addMaterial(Material&& mat) {
    _materials.emplace_back(std::forward<Material>(mat));
}

void Scene::addGeometry(Triangle&& triangle)
{
    _triangles.emplace_back(std::forward<Triangle>(triangle));
}

void Scene::compileGeometry()
{
    bool computeSmoorthNormals = false;
    for(const auto& mesh : _goemetryObjects ) {
        // Compute the smoorth normals if we got any material defined
        if(!_materials.empty()) {
            computeSmoorthNormals = computeSmoorthNormals || _materials[mesh.MaterialIndex].SmoothShading;
        } else {
            _materials.push_back(Material());
        }

        for (auto& triangleIndex : mesh.TriangleIndexes) {
            _triangles.emplace_back(
                                *(mesh.Vertices.at(get<0>(triangleIndex)).get()),
                                *(mesh.Vertices.at(get<1>(triangleIndex)).get()),
                                *(mesh.Vertices.at(get<2>(triangleIndex)).get()),
                                _materials[mesh.MaterialIndex]
            );        
        } 
    }

    for (auto& triangle : _triangles) {
        if(computeSmoorthNormals) {
            triangle.normalizeVertices();
        }

        _aabbox.expandBox(triangle);
    }
    
}

bool Scene::intersectAABB(const Ray& ray) const
{
    return _aabbox.checkIntersection(ray);
}

bool Scene::intersect(const Ray& ray, Intersaction* intersaction) const
{
    if((!intersaction && !ray.shadow()) || (intersaction && ray.shadow())) {
        assert(false);
        return false;
    }

    float minPointDistance = ray.tMax();
    for (const auto& triangle : triangles()) {
        float rayProj = ray.direction().dot(triangle.normal());
        // If generated ray is not paralel to triangle plane
        if (!Utils::equals(rayProj, 0.0f)) {
            float distance = triangle.distance(ray.origin());
            float t = distance / rayProj;
            // Check if triangle is not behind the ray
            if (t > 0.f)
            {
                Vector p = ray.origin() + (ray.direction() * t);
                if (triangle.checkIntersaction(p)) {
                    // Check if point is more close to the origin
                    if(t < minPointDistance) {
                        if (intersaction) {
                            minPointDistance = t;
                            intersaction->Point = p;
                            intersaction->Triangle = &triangle;
                        } 
                        // Shadow rays should not intersect with transparent objects.
                        else if(ray.shadow() && triangle.metrial().Type != Material::Type::Refractive) {
                            // Found trangle between the light and shadow-ray, no need to check for more
                            return true;
                        }
                    }
                }
            }
        }
    }

    return intersaction && intersaction->Triangle != nullptr;
}
