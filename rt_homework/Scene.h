#pragma once

#include "Shape.h"
#include "Camera.h"
#include <memory>
#include <vector>

class Ray;
struct Intersaction;

using TriangleIndexes = std::vector<std::tuple<int, int, int>>;
using Vertices = std::vector<std::unique_ptr<Vertex>>;

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
	int MaxDepth = 7; 
	float Bias = 0.0001f;
	float RefractionBias = 0.0001f;
};

struct Material
{
	enum class Type
	{
		Diffuse,
		Reflective,
		Refractive,
		Constant
	};

	static constexpr const char* JSON_MATERIALS = "materials";
	static constexpr const char* JSON_MATERIALS_TYPE = "type";
	static constexpr const char* JSON_MATERIALS_ALBEDO= "albedo";
	static constexpr const char* JSON_MATERIALS_SMOOTH = "smooth_shading";
	static constexpr const char* JSON_MATERIALS_IOR = "ior";

	Type Type = Type::Diffuse;
	bool SmoothShading = false;
	Vector Albedo{ 1.f, 1.f, 1.f };
	
	// Set default air IOR
	float IOR = 1.f;
};


struct Mesh
{
	static constexpr const char* JSON_OBJECTS = "objects";
	static constexpr const char* JSON_OBJECTS_VERTICES = "vertices";
	static constexpr const char* JSON_OBJECTS_TRIANGLES = "triangles";
	static constexpr const char* JSON_OBJECTS_MATERIAL = "material_index";
	

	Vertices Vertices;
	TriangleIndexes TriangleIndexes;
	int MaterialIndex = 0;
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

struct Object
{
	std::vector<int> _triangles;
};

class Scene
{
public:
	using Triangles = std::vector<Triangle>;
	using Objects = std::vector<Object>;

	using Lights = std::vector<Light>;
	using Materials = std::vector<Material>;
	using GeometryObjects = std::vector<Mesh>;

	Scene() = default;
	Scene(int imageWidth, int imageHeight);
	void addLight(Light&& light);
	void addMesh(Mesh&& mesh);
	void addMaterial(Material&& mat);
	void addGeometry(Triangle&& triangle);
	void compileGeometry();
	bool loadScene(const std::string filename);

	// When we want to update the settings
	Camera& camera() { return _camera; }
	Settings& settings() { return _settings; }

	const Camera& camera() const { return _camera; }
	const Settings& settings() const { return _settings; }
	const std::string& name() const { return _sceneFile; }
	const Triangles& triangles() const { return _triangles; }
	const Lights& lights() const { return _lights; }

	bool intersect(const Ray& ray, Intersaction* intersaction = nullptr) const;

private:
	void cleanup();

private:
	Triangles _triangles;
	Camera _camera;
	Settings _settings;
	GeometryObjects _goemetryObjects;
	Lights _lights;
	Materials _materials;
	std::string _sceneFile;
};