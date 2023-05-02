#include "Scene.h"
#include "Common.h"
#include "JsonParser.h"

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
        // Compute the smoorth normals
        computeSmoorthNormals = computeSmoorthNormals || _materials[mesh.MaterialIndex].SmoothShading;
        for (auto& triangleIndex : mesh.TriangleIndexes) {
            _triangles.emplace_back(
                                *(mesh.Vertices.at(get<0>(triangleIndex)).get()),
                                *(mesh.Vertices.at(get<1>(triangleIndex)).get()),
                                *(mesh.Vertices.at(get<2>(triangleIndex)).get()),
                                _materials[mesh.MaterialIndex]
            );        
        } 
    }

    if (computeSmoorthNormals) {
        for (auto& triangle : _triangles) {
            triangle.normalizeVertices();
        }
    }
}
