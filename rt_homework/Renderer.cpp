#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"
#include "Ray.h"
#include "Utils.h"
#include "FrameBuffer.h"

#include <chrono>
#include <assert.h>

Renderer::Renderer(Scene& scene) :
    _scene(scene)
{
}

void Renderer::renderScene(const std::string& filename)
{
    auto timeStart = std::chrono::high_resolution_clock::now();

    const Settings& settings = _scene.settings();
    FrameBuffer framebuffer(settings.ImageWidth, settings.ImageHeight);
    for (int y = 0; y < settings.ImageHeight; ++y) {
        for (int x = 0; x < settings.ImageWidth; ++x) {
            framebuffer.push(castRay((float)x, (float)y));
        }
    }

    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
    std::cout << "Render " << _scene.name() << " done in: " << passedTime << "ms" << std::endl;

    timeStart = std::chrono::high_resolution_clock::now();

    // Pixels rendered so write to file
    PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
    file.writeFrameBuffer(framebuffer);
	
    timeEnd = std::chrono::high_resolution_clock::now();
    passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
    std::cout << "Write " << filename << " done in: " << passedTime << "ms" << std::endl;
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
        float rayProj = ray.direction().dot(triangle.normal());
        // If generated ray is not paralel to triangle plane
        if (!Utils::equals(rayProj, 0.0f)) {
            float distance = triangle.distance(ray.origin());
            float t = distance / rayProj;
            // Check if triangle is not behind the ray
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
    // Trace primary ray
    Intersaction intersaction;
    const Ray ray(_scene.camera().position(), primaryRayDirection(x, y));
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

Vector Renderer::primaryRayDirection(float x, float y) const
{
    const Camera& camera = _scene.camera();
    const Settings& settings = _scene.settings();
    
    // Calculate coordinates in 2d space
    Vector rayDirection(x, y, -camera.planeDistance());
    rayDirection = rayDirection.getCenter();
    rayDirection = rayDirection.getNDC((float)settings.ImageWidth, (float)settings.ImageHeight);
    rayDirection = rayDirection.getWorldSpace();

    // Consider the aspect ratio
    float aspectRatio = (float)settings.ImageWidth / (float)settings.ImageHeight;
    rayDirection.setX(rayDirection.x() * aspectRatio);

    // Set camera rotation matrix
    rayDirection.normalize();
    rayDirection = camera.rotation() * rayDirection;

    return rayDirection;
}

