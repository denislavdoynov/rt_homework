#pragma once

#include "Scene.h"

constexpr float ROUNDING_ERROR_FLOAT = 0.0001f;

class Ray
{
public:
    Ray() = delete;
    Ray(const Scene::Triangles& triangles);
    const Triangle* trace(const Vector& origin, const Vector& rayDirection, Vector& hitPoint);
    void setShadowRay(bool shadowRay) { _shadowRay = shadowRay; } 

private:
    inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT);

    const Scene::Triangles& _triangles;
    bool _shadowRay = false;
};

