#pragma once

#include "BucketManager.h"
#include "Vector.h"
#include "ThreadManager.h"
#include "Utils.h"
#include <memory>
#include <string>

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
	void abort();
	
	friend class ThreadManager;

protected:
	Color castCameraRay(int pixelIdx) const;

private:	
	Color castRay(const Ray& ray, const Triangles& triangles) const;
	Color shadeDeffuse(const Ray& ray, const Intersaction& intersaction) const;
	Color shadeReflective(const Ray& ray, const Intersaction& intersaction) const;
	Color shadeRefractive(const Ray& ray, const Intersaction& intersaction) const;

	Vector primaryRayDirection(int pixelIdx) const;
	
	bool refractRayDirection(const Vector& rayDir, Vector& surfNormal, const float& ior, Vector& refRayDir) const;
	Color reflectiveRayDirection(const Vector& rayDir, const Vector& surfNormal) const;

	int writeOutputFile(const std::string& filename, const FrameBuffer& framebuffer);

private:
	const std::shared_ptr<Scene> _scenePtr;
	const Scene& _scene;
	BucketManager _bucketManager;
	ThreadManager _threadManager;
	int _totalElapsedTime = 0;
	bool _abort = false;
};

