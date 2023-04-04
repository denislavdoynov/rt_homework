#include "Scene.h"
#include "Common.h"

Scene::Scene() :
    Drawable(IMAGE_WIDTH, IMAGE_HEIGHT) 
{
}

void Scene::addGeometry(const Triangle& triangle)
{
    _triangles.emplace_back(triangle);
}

void Scene::setCamera(const Vector& origin)
{
    _camera = origin;
}

//! returns if a equals b, taking possible rounding errors into account
bool Scene::equals(const float a, const float b, const float tolerance)
{
    return (a + tolerance >= b) && (a - tolerance <= b);
}

Color Scene::pixelColor(float x, float y) 
{
    Vector rayDirection(x, y, -1);
    rayDirection = rayDirection.getCenter().getNDC(_imageWidth, _imageHeight).getWorldSpace();
  
    // Consider the aspect ratio
    float aspectRatio = (float)_imageWidth / (float)_imageHeight;
    rayDirection.X *= aspectRatio;

    rayDirection = rayDirection.normalize();

    const Triangle* intersactionTriangle = nullptr;
    float pointDistance = 0.f;
    for(const auto& triangle : _triangles ) {
         float normalDotRayDir = rayDirection.dot(triangle.normal());
         // If generated ray is not paralel to triangle plane
        if(!equals(normalDotRayDir, 0.0f)) {
            // If R is towards the triangle’s plane
            if(normalDotRayDir < 0) {
                float t = (triangle.normal().dot(_camera) + triangle.distance()) / normalDotRayDir;
                Vector vP = _camera + (rayDirection * t);
                if(triangle.checkIntersaction(vP)) {
                    // Check if point is more close to the origin
                    if (equals(pointDistance, 0.f) || t < pointDistance) {
                        pointDistance = t;
                        intersactionTriangle = &triangle;
                    }
                }
             }

        }
    }
    if(intersactionTriangle != nullptr) {
        return intersactionTriangle->color();
    }

    return rayDirection.toColor(MAX_COLOR_COMPONENT);
}