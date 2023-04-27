#include "Ray.h"
#include "Utils.h"


Ray::Ray(const Scene::Triangles& triangles) : 
    _triangles(triangles)
{
}

// Make sure rayDirection is normalized
const Triangle* Ray::trace(const Vector& origin, const Vector& rayDirection, Vector& hitPoint)
{
    float minPointDistance = 0.f;
    const Triangle* intersaction = nullptr;
    for (const auto& triangle : _triangles) {
        float normalDotRayDir = rayDirection.dot(triangle.normal());
        // If generated ray is not paralel to triangle plane
        if (!equals(normalDotRayDir, 0.0f)) {
            // If R is towards the triangle’s plane
            float distance = triangle.distance(origin);
            if (/*_shadowRay || */distance < 0) {
                float t = distance / normalDotRayDir;
                Vector p = origin + (rayDirection * t);
                if (triangle.checkIntersaction(p)) {
                    // Check if point is more close to the origin
                    if(_shadowRay) {
                        return &triangle;
                    }

                    if (!intersaction || t < minPointDistance) {
                        minPointDistance = t;
                        hitPoint = p;
                        intersaction = &triangle;
                    }
                }
            }
        }
    }

    return intersaction;
}

//! returns if a equals b, taking possible rounding errors into account
bool Ray::equals(const float a, const float b, const float tolerance)
{
    return (a + tolerance >= b) && (a - tolerance <= b);
}
