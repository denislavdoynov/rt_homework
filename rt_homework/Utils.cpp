#include "Utils.h"
#include "Vector.h"
#include <cmath>

float Utils::getArea(float radius)
{
	return 4 * PI * radius * radius;
}

//! returns if a equals b, taking possible rounding errors into account
bool Utils::equals(const float a, const float b, const float tolerance)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

float Utils::clamp(const float& lo, const float& hi, const float& v)
{
	return std::max(lo, std::min(hi, v));
}

float Utils::fresnel(const Vector& rayDir, const Vector& surfNormal) {
    return 0.5f * std::pow(1.f + rayDir.dot(surfNormal), 5.f);
}