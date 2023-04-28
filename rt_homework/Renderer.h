#pragma once
#include "Utils.h"

class Scene;
class Ray;

struct Intersaction
{
	Point Point;
	const Triangle* Triangle = nullptr;
};

class Renderer
{
public:
	Renderer(Scene& scene);
	void renderScene(const std::string& filename);

private:
	Color castRay(float x, float y) const;
	bool traceShadow(const Ray& ray) const;
	bool tracePrimary(const Ray& ray, Intersaction& intersaction) const;
	
private:
	Vector primaryRayDirection(float x, float y) const;
	bool trace(const Ray& ray, Intersaction* intersaction, bool shadowRay = false) const;

private:
	Scene& _scene;
	float _shadowBias = 0.00001f;
	Vector _alberdo{1.0f, 0.5f, 0.5f};
};

