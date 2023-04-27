#pragma once

#include "Shape.h"
#include "Camera.h"
#include <vector>

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

	struct Light
	{
		static constexpr const char* JSON_LIGHTS = "lights";
		static constexpr const char* JSON_LIGHTS_INTENSITY = "intensity";
		static constexpr const char* JSON_LIGHTS_POSITION = "position";

		Vector Position;
		float Intensity = 0.f;
	};

	using Lights = std::vector<Light>;
	using GeometryObjects = std::vector<Mesh>;

	Scene() = default;
	Scene(int imageWidth, int imageHeight);
	void addLight(Light&& light);
	void addMesh(Mesh&& mesh);
	void addGeometry(const Triangle& triangle);
	void compileGeometry();
	bool loadScene(const std::string filename);

	Camera& camera() { return _camera; }
	Settings& settings() { return _settings; }
	const Triangles& triangles() const { return _triangles; }
	const Lights& lights() const { return _lights; }

private:
	void cleanup();

private:
	Triangles _triangles;
	Camera _camera;
	Settings _settings;
	GeometryObjects _goemetryObjects;
	Lights _lights;
};