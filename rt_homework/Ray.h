#pragma once

#include "Utils.h"

struct Intersaction
{
	Point Point;
	const Triangle* Triangle = nullptr;
};

class Ray
{
public:
    Ray(const Vector& origin, const Vector& direction, int depth);

    const Vector& origin() const { return _origin; }
    const Vector& direction() const { return _direction; }
    int depth() const { return _depth; }

    virtual const bool shadow() const = 0;

private:
    const Vector _origin;
    const Vector _direction;
    int _depth = 0;
};

class ShadowRay : public Ray
{
public:
    ShadowRay(const Vector& origin, const Vector& direction, int depth = 0) : Ray(origin, direction, depth) {}
    const bool shadow() const override { return true; }
};

class PrimaryRay : public Ray
{
public:
    PrimaryRay(const Vector& origin, const Vector& direction, int depth = 0) : Ray(origin, direction, depth) {}
    const bool shadow() const override { return false; }
};