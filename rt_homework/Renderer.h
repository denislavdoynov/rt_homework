#pragma once
#include "Utils.h"
#include "Ray.h"
#include <vector>
#include <thread>
#include <sstream>

class Scene;
class FrameBuffer;

struct Intersaction
{
	Point Point;
	const Triangle* Triangle = nullptr;
};

class Renderer
{
public:
	Renderer(Scene& scene);
	int renderScene(const std::string& filename, FrameBuffer* buffer = nullptr, std::stringstream* log = nullptr);

private:
	Color castRay(const Ray& ray, int& depth) const;
	bool traceShadow(const Ray& ray) const;
	bool tracePrimary(const Ray& ray, Intersaction& intersaction) const;
	
private:
	Vector primaryRayDirection(int pixelIdx) const;
	bool trace(const Ray& ray, Intersaction* intersaction, bool shadowRay = false) const;

private:
	Scene& _scene;
};

