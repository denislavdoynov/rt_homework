#include "Scene.h"
#include "Common.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

using namespace rapidjson;
using namespace std;

Scene::Scene(int imageWidth, int imageHeight)
{
    _settings.ImageWidth = imageWidth;
    _settings.ImageHeight = imageHeight;
}
Document Scene::getJsonDoc(const std::string filename)
{
    std::ifstream ifs(filename);
    IStreamWrapper isw(ifs);

    // Parse the JSON document
    Document doc;
    doc.ParseStream(isw);
    return doc;
}

Matrix Scene::loadMatrix(const Value::ConstArray& vecArray)
{   
    if (vecArray.Size() < 9) {
        assert(false);
        return Matrix();
    }

    return { 
        vecArray[0].GetFloat(), vecArray[1].GetFloat(), vecArray[2].GetFloat(),
        vecArray[3].GetFloat(), vecArray[4].GetFloat(), vecArray[5].GetFloat(),
        vecArray[6].GetFloat(), vecArray[7].GetFloat(), vecArray[8].GetFloat() 
    };
}

void Scene::loadTriangles(const Value::ConstArray& vecArray, TriangleIndexes& triangles)
{
    for (unsigned int i = 0; i < vecArray.Size(); i += 3) {
        if ((i + 2) >= vecArray.Size())
            break;

        triangles.emplace_back( 
            vecArray[i].GetInt(),
            vecArray[i+1].GetInt(),
            vecArray[i+2].GetInt()
        );
    }
}

void Scene::loadVectors(const Value::ConstArray& vecArray, Vertices& vertices)
{
    for(unsigned int i = 0; i < vecArray.Size(); i += 3) {
        if((i+2) >= vecArray.Size() )
            break;

        vertices.emplace_back(Vector(
            vecArray[i].GetFloat(),
            vecArray[i+1].GetFloat(),
            vecArray[i+2].GetFloat() 
        ));
    }
}

Vector Scene::loadVector(const Value::ConstArray& vecArray)
{
    Vector res;
    if(vecArray.Size() < 3) {
        assert(false);
        return res;
    }

    res.setX(vecArray[0].GetFloat());
    res.setY(vecArray[1].GetFloat());
    res.setZ(vecArray[2].GetFloat());

    return res;
}

void Scene::cleanup()
{
    _goemetryObjects.clear();
    _triangles.clear();
}

bool Scene::loadScene(const std::string filename)
{
    cleanup();

    Document doc = getJsonDoc(filename);
    if(doc.HasParseError())
        return false;

    const auto& settingsVal = doc.FindMember(Settings::JSON_SETTINGS)->value;
    if(!settingsVal.IsNull() && settingsVal.IsObject()) {
        const auto& bgColor = settingsVal.FindMember(Settings::JSON_SETTINGS_BG_COLOR)->value;
        if(bgColor.IsArray()) {
            _settings.BackGroundColor = loadVector(bgColor.GetArray());
        }

        const auto& imageSettings = settingsVal.FindMember(Settings::JSON_SETTINGS_IMAGE_SETTINGS)->value;
        if (!imageSettings.IsNull() && imageSettings.IsObject()) {
            _settings.ImageWidth = imageSettings.FindMember(Settings::JSON_SETTINGS_WIDTH)->value.GetInt();
            _settings.ImageHeight = imageSettings.FindMember(Settings::JSON_SETTINGS_HEIGHT)->value.GetInt();
        }
    }

    const auto& camera = doc.FindMember(Camera::JSON_CAMERA)->value;
    if (!camera.IsNull() && camera.IsObject()) {
        const auto& matrix = camera.FindMember(Camera::JSON_CAMERA_MATRIX)->value;
        if (matrix.IsArray()) {
            _camera.setMatrix(loadMatrix(matrix.GetArray()));;
        }

        const auto& position = camera.FindMember(Camera::JSON_CAMERA_POSITION)->value;
        if (position.IsArray()) {
            _camera.setPosition(loadVector(position.GetArray()));
        }
    }

    const auto& objects = doc.FindMember(Mesh::JSON_OBJECTS)->value;
    if (!objects.IsNull() && objects.IsArray()) {
        for(const auto& object : objects.GetArray()) {
            Mesh mesh;
            const auto& vertices = object.FindMember(Mesh::JSON_OBJECTS_VERTICES)->value;
            if (vertices.IsArray()) {
                loadVectors(vertices.GetArray(), mesh.Vertices);
            }

            const auto& triangles = object.FindMember(Mesh::JSON_OBJECTS_TRIANGLES)->value;
            if (triangles.IsArray()) {
                loadTriangles(triangles.GetArray(), mesh.TriangleIndexes);
            }

            _goemetryObjects.emplace_back(std::move(mesh));
        }
    }

    compileGeometry();

    return true;
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
