#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"
#include "Ray.h"
#include "Utils.h"
#include <chrono>
#include <assert.h>

Renderer::Renderer(Scene& scene) :
    _scene(scene)
{
}

double Renderer::renderScene(const std::string& filename)
{
    auto timeStart = std::chrono::high_resolution_clock::now();

    const Scene::Settings& settings = _scene.settings();
    std::vector<Vector> framebuffer;    
    framebuffer.reserve(settings.ImageWidth * settings.ImageHeight);
    for (int y = 0; y < settings.ImageHeight; ++y) {
        for (int x = 0; x < settings.ImageWidth; ++x) {
            framebuffer.emplace_back(castRay((float)x, (float)y));
        }
    }
    	
    // Pixels rendered so write to file
    PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
    file.writeFrameBuffer(framebuffer);
	
    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();

    return passedTime;
}

bool Renderer::traceShadow(const Ray& ray) const 
{
    return trace(ray, nullptr, true);
}

bool Renderer::tracePrimary(const Ray& ray, Intersaction& intersaction) const
{
    return trace(ray, &intersaction, false);
}

bool Renderer::trace(const Ray& ray, Intersaction* intersaction, bool shadowRay) const
{
    if((!intersaction && !shadowRay) || (intersaction && shadowRay)) {
        assert(false);
        return false;
    }

    float minPointDistance = std::numeric_limits<float>::max();
    for (const auto& triangle : _scene.triangles()) {
        float normalDotRayDir = ray.direction().dot(triangle.normal());
        // If generated ray is not paralel to triangle plane
        if (!Math::equals(normalDotRayDir, 0.0f)) {
            float distance = triangle.distance(ray.origin());
            float t = distance / normalDotRayDir;
            // Check if triangle is not not behind the ray
            if (t > 0.f)
            {
                Vector p = ray.origin() + (ray.direction() * t);
                if (triangle.checkIntersaction(p)) {
                    if (shadowRay) {
                        return true;
                    }

                    // Check if point is more close to the origin
                    if (intersaction && t <= minPointDistance) {
                        minPointDistance = t;
                        intersaction->Point = p;
                        intersaction->Triangle = &triangle;
                    }
                }
            }
        }
    }

    return intersaction && intersaction->Triangle != nullptr;
}

Color Renderer::castRay(float x, float y) const
{
    // Primery ray direction
    Vector rayDirection = generateRayDirection(x, y);

    // Trace primary ray
    Intersaction intersaction;
    const Ray ray(_scene.camera().position(), rayDirection);
    if (tracePrimary(ray, intersaction)) {
        Color finalColor;
        for (const auto& light : _scene.lights()) {
            // Shadow ray direction
            Color lightDir;
            float area = light.getIllumination(intersaction.Point, lightDir);
     
            // Trace shadow ray
            Ray shadowRay(intersaction.Point + intersaction.Triangle->normal() * _shadowBias, lightDir);
            if(!traceShadow(shadowRay)) {                
                float cosLaw = std::max(0.f, lightDir.dot(intersaction.Triangle->normal()));
                float colorCorrection = light.Intensity / area * cosLaw;
                Vector lightContribution(_alberdo);
                lightContribution *= colorCorrection;
                finalColor += lightContribution;
            }
        }

       return finalColor;
    }

    return _scene.settings().BackGroundColor;
}

Vector Renderer::generateRayDirection(float x, float y) const
{
    const Camera& camera = _scene.camera();
    Vector rayDirection(x, y, -camera.planeDistance());
    const Scene::Settings& settings = _scene.settings();
    rayDirection = rayDirection.getCenter();
    rayDirection = rayDirection.getNDC((float)settings.ImageWidth, (float)settings.ImageHeight);
    rayDirection = rayDirection.getWorldSpace();

    // Consider the aspect ratio
    float aspectRatio = (float)settings.ImageWidth / (float)settings.ImageHeight;
    rayDirection.setX(rayDirection.x() * aspectRatio);

    // Set camera rotation matrix
    rayDirection = camera.rotation() * rayDirection;
    rayDirection = rayDirection.normalize();

    return rayDirection;
}

