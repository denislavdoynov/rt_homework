#include "Scene.h"
#include "Common.h"
#include "JsonParser.h"
#include "Ray.h"

using namespace std;

Scene::Scene() 
    : _accTree(_triangles)
{
}

Scene::Scene(int imageWidth, int imageHeight) 
    : _accTree(_triangles)
{
    _settings.ImageWidth = imageWidth;
    _settings.ImageHeight = imageHeight;
}

Scene::~Scene() 
{
    cleanup();
}

void Scene::cleanup()
{
    _goemetryObjects.clear();
    for(auto t : _triangles) {
        delete t;
    }
    _triangles.clear();
    _lights.clear();
    _sceneFile.clear();
    _materials.clear();
    _accTree.clear();
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

void Scene::addGeometry(Triangle* triangle)
{
    _triangles.emplace_back(triangle);
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
            _triangles.emplace_back(new Triangle(  *(mesh.Vertices.at(get<0>(triangleIndex)).get()),
                                                        *(mesh.Vertices.at(get<1>(triangleIndex)).get()),
                                                        *(mesh.Vertices.at(get<2>(triangleIndex)).get()),
                                                        _materials[mesh.MaterialIndex]));
        } 
    }
    AABBox mainAABBox;
    for (const auto triangle : _triangles) {
        if(computeSmoorthNormals) {
            triangle->normalizeVertices();
        }

        mainAABBox.expandBox(*triangle);
    }
    _accTree.clear();
    _accTree.buildTree(mainAABBox);    
}

bool Scene::intersectAABBox(const Ray& ray, Triangles& triangles) const
{
    return _accTree.checkIntersection(ray, triangles);
}

bool Scene::intersect(const Ray& ray, const Triangles& triangles, Intersaction* intersaction) const
{
    if((!intersaction && !ray.shadow()) || (intersaction && ray.shadow())) {
        assert(false);
        return false;
    }

    float minPointDistance = ray.tMax();
    for (const auto triangle : triangles) {
        float rayProj = ray.direction().dot(triangle->normal());
        // If generated ray is not paralel to triangle plane
        if (!Utils::equals(rayProj, 0.0f)) {
            float distance = triangle->distance(ray.origin());
            float t = distance / rayProj;
            // Check if triangle is not behind the ray
            if (t > 0.f)
            {
                Vector p = ray.origin() + (ray.direction() * t);
                if (triangle->checkIntersaction(p)) {
                    // Check if point is more close to the origin
                    if(t < minPointDistance) {
                        if (intersaction) {
                            minPointDistance = t;
                            intersaction->Point = p;
                            intersaction->Triangle = triangle;
                        } 
                        // Shadow rays should not intersect with transparent objects.
                        else if(ray.shadow() && triangle->metrial().Type != Material::Type::Refractive) {
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
