#pragma once

#include <limits>
#include <vector>

constexpr float PI = 3.14159265358979f;
constexpr float ROUNDING_ERROR_FLOAT = 0.0001f;
constexpr float MAX_FLOAT = std::numeric_limits<float>::max();
constexpr float MIN_FLOAT = -MAX_FLOAT;

class Triangle;
using Triangles = std::vector<Triangle>;

class Vector;
class Utils
{
public:
	static float getArea(float radius);
	//! returns if a equals b, taking possible rounding errors into account
	static bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT);

	static float clamp(const float& lo, const float& hi, const float& v);

	static float fresnel(const Vector& rayDir, const Vector& surfNormal);
};