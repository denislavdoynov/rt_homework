#pragma once

#include "Scene.h"
#include <string>
#include "rapidjson/document.h"

class JsonParser
{
public:
	JsonParser(const std::string& filename);
    bool load(Scene& scene);

private:
	rapidjson::Document getJsonDoc();
	Vector loadVector(const rapidjson::Value::ConstArray& vecArray);
	void loadVectors(const rapidjson::Value::ConstArray& vecArray, Scene::Vertices& vertices);
	void loadTriangles(const rapidjson::Value::ConstArray& vecArray, Scene::TriangleIndexes& triangles);
	Matrix loadMatrix(const rapidjson::Value::ConstArray& vecArray);

	std::string _jsonFile;
};


