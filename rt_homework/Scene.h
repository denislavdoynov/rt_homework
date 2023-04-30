#pragma once

#include "Shape.h"
#include "Camera.h"
#include <vector>

using TriangleIndexes = std::vector<std::tuple<int, int, int>>;
using Vertices = std::vector<Vector>;

struct Settings
{
	static constexpr const char* JSON_SETTINGS = "settings";
	static constexpr const char* JSON_SETTINGS_BG_COLOR = "background_color";
	static constexpr const char* JSON_SETTINGS_IMAGE_SETTINGS = "image_settings";
	static constexpr const char* JSON_SETTINGS_WIDTH = "width";
	static constexpr const char* JSON_SETTINGS_HEIGHT = "height";

	Color BackGroundColor;
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

	Point Position;
	float Intensity = 0.f;

	float getIllumination(const Point& point, Vector& lightDir) const {
		lightDir = Position - point;
		float radius = lightDir.magnitude();
		lightDir.normalize();
		return Utils::getArea(radius);

	}
};

class Scene
{
public:
	using Triangles = std::vector<Triangle>;
	using Lights = std::vector<Light>;
	using GeometryObjects = std::vector<Mesh>;

	Scene() = default;
	Scene(int imageWidth, int imageHeight);
	void addLight(Light&& light);
	void addMesh(Mesh&& mesh);
	void addGeometry(Triangle&& triangle);
	void compileGeometry();
	bool loadScene(const std::string filename);

	Camera& camera() { return _camera; }
	Settings& settings() { return _settings; }
	std::string& name() { return _sceneFile; }
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
	std::string _sceneFile;
};