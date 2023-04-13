#pragma once

#include "Shape.h"
#include "Camera.h"
#include <vector>
#include "rapidjson/document.h"

class Scene
{
public:
	using TriangleIndexes = std::vector<std::tuple<int, int, int>>;
	using Vertices = std::vector<Vector>;
	using Triangles = std::vector<Triangle>;

	struct Settings
	{
		static constexpr const char* JSON_SETTINGS = "settings";
		static constexpr const char* JSON_SETTINGS_BG_COLOR = "background_color";
		static constexpr const char* JSON_SETTINGS_IMAGE_SETTINGS = "image_settings";
		static constexpr const char* JSON_SETTINGS_WIDTH = "width";
		static constexpr const char* JSON_SETTINGS_HEIGHT = "height";

		Vector BackGroundColor;
		int ImageWidth = 0;
		int ImageHeight = 0;
	};

	struct Mesh
	{
		static constexpr const char* JSON_OBJECTS = "objects";
		static constexpr const char* JSON_OBJECTS_VERTICES = "vertices";
		static constexpr const char* JSON_OBJECTS_TRIANGLES = "triangles";

		Vertices Vertices;
		TriangleIndexes TriangleIndexes;
	};

	Scene() = default;
	Scene(int imageWidth, int imageHeight);
	void addGeometry(const Triangle& triangle);
	void compileGeometry();
	bool loadScene(const std::string filename);

	Camera& camera() { return _camera; }
	const Settings& settings() const { return _settings; }
	const Triangles& triangles() const { return _triangles; }

private:
	rapidjson::Document getJsonDoc(const std::string filename);
	Vector loadVector(const rapidjson::Value::ConstArray& vecArray);
	void loadVectors(const rapidjson::Value::ConstArray& vecArray, Vertices& vertices);
	void loadTriangles(const rapidjson::Value::ConstArray& vecArray, TriangleIndexes& triangles);
	Matrix loadMatrix(const rapidjson::Value::ConstArray& vecArray);

	void cleanup();

private:
	Triangles _triangles;
	Camera _camera;
	Settings _settings;

	std::vector<Mesh> _goemetryObjects;
};