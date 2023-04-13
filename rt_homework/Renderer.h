#pragma once
#include "Utils.h"
 
class Scene;

constexpr float ROUNDING_ERROR_FLOAT = 0.0001f;

class Renderer
{
public:
	Renderer(Scene& scene);
	void renderScene(const std::string& filename);

private:
	inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT);
	Color traceRay(float x, float y);

private:
	Scene& _scene;
};

