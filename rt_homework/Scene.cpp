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
}

bool Scene::loadScene(const std::string filename)
{
    cleanup();

    JsonParser file(filename);
    if(file.load(*this)) {
        compileGeometry();
        return true;
    }

    return false;
    
}
void Scene::addMesh(Mesh&& mesh)
{
    _goemetryObjects.emplace_back(mesh);
}

void Scene::addLight(Light&& light)
{
    _lights.emplace_back(light);
}

void Scene::addGeometry(const Triangle& triangle)
{
    _triangles.emplace_back(triangle);
}

void Scene::compileGeometry()
{
    for(const auto& mesh : _goemetryObjects ) {
        for (const auto& triangleIndex : mesh.TriangleIndexes) {
            _triangles.emplace_back(Triangle(
                mesh.Vertices.at(get<0>(triangleIndex)),
                mesh.Vertices.at(get<1>(triangleIndex)),
                mesh.Vertices.at(get<2>(triangleIndex))));
        }
    }
}
