#pragma once

#include "Utils.h"
#include <memory>

class Scene;
class FrameBuffer;
class Ray;
struct Intersaction;

class Renderer
{
public:
	Renderer(const Scene& scene);
	Renderer(std::shared_ptr<Scene>& scene);
	int renderScene(const std::string& filename, FrameBuffer* buffer = nullptr, std::stringstream* log = nullptr);

private:
	Color castRay(const Ray& ray) const;

private:
	Vector primaryRayDirection(int pixelIdx) const;
	
	bool refractRayDirection(const Vector& rayDir, const Vector& surfNormal, float eta, float cosThetaI, Vector& refRayDir) const;
	Color reflectiveRayDirection(const Vector& rayDir, const Vector& surfNormal) const;

private:
	const std::shared_ptr<Scene> _scenePtr;
	const Scene& _scene;
};

