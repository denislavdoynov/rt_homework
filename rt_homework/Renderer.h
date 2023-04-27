#pragma once
#include "Utils.h"
 
class Scene;

class Renderer
{
public:
	Renderer(Scene& scene);
	double renderScene(const std::string& filename);

private:
	inline float getArea(float radius) const;
	Vector castRay(float x, float y) const;

private:
	Scene& _scene;
	float _shadowBias = 10.5f;
	Vector _alberdo{1.0f, 0.5f, 0.5f};
};

