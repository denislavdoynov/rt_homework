#include "JsonParser.h"
#include "Scene.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

using namespace rapidjson;

JsonParser::JsonParser(const std::string& filename) : 
    _jsonFile(filename)
{
}

Document JsonParser::getJsonDoc()
{
    std::ifstream ifs(_jsonFile);
    IStreamWrapper isw(ifs);

    // Parse the JSON document
    Document doc;
    doc.ParseStream(isw);
    return doc;
}

Matrix JsonParser::loadMatrix(const Value::ConstArray& vecArray)
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

void JsonParser::loadTriangles(const Value::ConstArray& vecArray, TriangleIndexes& triangles)
{
    for (unsigned int i = 0; i < vecArray.Size(); i += 3) {
        if ((i + 2) >= vecArray.Size())
            break;

        triangles.emplace_back(
            vecArray[i].GetInt(),
            vecArray[i + 1].GetInt(),
            vecArray[i + 2].GetInt()
        );
    }
}

void JsonParser::loadVectors(const Value::ConstArray& vecArray, Vertices& vertices)
{
    for (unsigned int i = 0; i < vecArray.Size(); i += 3) {
        if ((i + 2) >= vecArray.Size())
            break;

        vertices.emplace_back(Vector(
            vecArray[i].GetFloat(),
            vecArray[i + 1].GetFloat(),
            vecArray[i + 2].GetFloat()
        ));
    }
}

Vector JsonParser::loadVector(const Value::ConstArray& vecArray)
{
    Vector res;
    if (vecArray.Size() < 3) {
        assert(false);
        return res;
    }

    res.setX(vecArray[0].GetFloat());
    res.setY(vecArray[1].GetFloat());
    res.setZ(vecArray[2].GetFloat());

    return res;
}

bool JsonParser::load(Scene& scene)
{
    Document doc = getJsonDoc();
    if (doc.HasParseError())
        return false;

    auto& sceneSettings = scene.settings();
    auto& sceneCamera = scene.camera();

    const auto& settingsVal = doc.FindMember(Settings::JSON_SETTINGS)->value;
    if (!settingsVal.IsNull() && settingsVal.IsObject()) {
        const auto& bgColor = settingsVal.FindMember(Settings::JSON_SETTINGS_BG_COLOR)->value;
        if (bgColor.IsArray()) {
            sceneSettings.BackGroundColor = loadVector(bgColor.GetArray());
        }

        const auto& imageSettings = settingsVal.FindMember(Settings::JSON_SETTINGS_IMAGE_SETTINGS)->value;
        if (!imageSettings.IsNull() && imageSettings.IsObject()) {
            sceneSettings.ImageWidth = imageSettings.FindMember(Settings::JSON_SETTINGS_WIDTH)->value.GetInt();
            sceneSettings.ImageHeight = imageSettings.FindMember(Settings::JSON_SETTINGS_HEIGHT)->value.GetInt();
        }
    }

    const auto& camera = doc.FindMember(Camera::JSON_CAMERA)->value;
    if (!camera.IsNull() && camera.IsObject()) {
        const auto& matrix = camera.FindMember(Camera::JSON_CAMERA_MATRIX)->value;
        if (matrix.IsArray()) {
            sceneCamera.setMatrix(loadMatrix(matrix.GetArray()));;
        }

        const auto& position = camera.FindMember(Camera::JSON_CAMERA_POSITION)->value;
        if (position.IsArray()) {
            sceneCamera.setPosition(loadVector(position.GetArray()));
        }
    }

    const auto& objects = doc.FindMember(Mesh::JSON_OBJECTS)->value;
    if (!objects.IsNull() && objects.IsArray()) {
        for (const auto& object : objects.GetArray()) {
            Mesh mesh;
            const auto& vertices = object.FindMember(Mesh::JSON_OBJECTS_VERTICES)->value;
            if (vertices.IsArray()) {
                loadVectors(vertices.GetArray(), mesh.Vertices);
            }

            const auto& triangles = object.FindMember(Mesh::JSON_OBJECTS_TRIANGLES)->value;
            if (triangles.IsArray()) {
                loadTriangles(triangles.GetArray(), mesh.TriangleIndexes);
            }

            scene.addMesh(std::move(mesh));            
        }
    }

    const auto& lights = doc.FindMember(Light::JSON_LIGHTS)->value;
    if (!lights.IsNull() && lights.IsArray()) {
        for (const auto& light : lights.GetArray()) {
            const auto& position = light.FindMember(Light::JSON_LIGHTS_POSITION)->value;
            if (position.IsArray()) {
                Light sceneLight;
                sceneLight.Position = loadVector(position.GetArray());
                sceneLight.Intensity = light.FindMember(Light::JSON_LIGHTS_INTENSITY)->value.GetFloat();
                scene.addLight(std::move(sceneLight));
            }           
        }
    }



    return true;
}
