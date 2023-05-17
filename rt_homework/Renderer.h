#pragma once

#include "Utils.h"
#include <memory>

class Scene;
class FrameBuffer;
class Ray;

struct Intersaction
{
	Point Point;
	const Triangle* Triangle = nullptr;
};

class Renderer
{
public:
	Renderer(const Scene& scene);
	Renderer(std::shared_ptr<Scene>& scene);
	int renderScene(const std::string& filename, FrameBuffer* buffer = nullptr, std::stringstream* log = nullptr);

private:
	Color castRay(const Ray& ray, int& depth) const;
	bool trace(const Ray& ray, Intersaction* intersaction = nullptr) const;

private:
	Vector primaryRayDirection(int pixelIdx) const;
	
private:
	const std::shared_ptr<Scene> _scenePtr;
	const Scene& _scene;
};

