#pragma once
#include "Utils.h"
#include "Ray.h"
#include <vector>
#include <thread>

class Scene;

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
	Color castRay(const Ray& ray, int& depth) const;
	bool traceShadow(const Ray& ray) const;
	bool tracePrimary(const Ray& ray, Intersaction& intersaction) const;
	
private:
	Vector primaryRayDirection(int pixelIdx) const;
	bool trace(const Ray& ray, Intersaction* intersaction, bool shadowRay = false) const;

private:
	Scene& _scene;
	float _shadowBias = 0.00001f;
};

