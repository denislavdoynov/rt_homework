#pragma once

#include "Shape.h"

#include <vector>

constexpr float ROUNDING_ERROR_FLOAT = 0.0001f;

class Scene : public Drawable
{
public:
	Scene();
	void addGeometry(const Triangle& triangle);
	void setCamera(const Vector& origin);

	Color pixelColor(float x, float y) override;

private:
	inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT);

private:
	std::vector<Triangle> _triangles;
	Vector _camera{0, 0, 0};
};