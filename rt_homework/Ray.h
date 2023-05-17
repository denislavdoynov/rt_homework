#pragma once

#include "Utils.h"

class Ray
{
public:
    Ray(const Vector& origin, const Vector& direction);

    const Vector& origin() const { return _origin; }
    const Vector& direction() const { return _direction; }
    
    virtual const bool shadow() const = 0;

private:
    const Vector _origin;
    const Vector _direction;
};

class ShadowRay : public Ray
{
public:
    ShadowRay(const Vector& origin, const Vector& direction) : Ray(origin, direction) {}
    const bool shadow() const override { return true; }
};

class PrimaryRay : public Ray
{
public:
    PrimaryRay(const Vector& origin, const Vector& direction) : Ray(origin, direction) {}
    const bool shadow() const override { return false; }
};