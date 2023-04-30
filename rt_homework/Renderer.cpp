#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"
#include "Ray.h"
#include "Utils.h"
#include "FrameBuffer.h"

#include <chrono>
#include <assert.h>
#include <iostream>

#define MULTI_THREADED

Renderer::Renderer(Scene& scene) :
    _scene(scene)
{
}

void Renderer::renderScene(const std::string& filename)
{
    auto timeStart = std::chrono::high_resolution_clock::now();

    const Settings& settings = _scene.settings();
    FrameBuffer framebuffer(settings.ImageWidth * settings.ImageHeight);

#ifdef MULTI_THREADED
    std::cout << "Multi-threaded render started" << std::endl;
    // Incease thread count on CPUs with more cores
    const int threadCount = 10;
    std::vector<std::thread> renderThreads;
    renderThreads.reserve(threadCount);
    for (int threadId = 0; threadId < threadCount; ++threadId) {
        renderThreads.emplace_back([this, threadId, threadCount, &framebuffer]() {
            int step = framebuffer.size() / threadCount;
            int startIndex = step * threadId;
            int endIndex = (threadId+1 == threadCount) ? framebuffer.size() : (startIndex + step - 1);
            for (int i = startIndex; i < endIndex; ++i) {
                Ray ray(_scene.camera().position(), primaryRayDirection(i));
                framebuffer[i] = castRay(ray);
            }
        });
    }

    // Wait for all threads to finish
    for (auto& th : renderThreads) {
        th.join();
    }

#else
    std::cout << "Single-threaded render started" << std::endl;
     for (int i = 0; i < pixelSize; ++i) {
         Ray ray(_scene.camera().position(), primaryRayDirection(i));
         framebuffer[i] = castRay(ray);
    }
#endif

    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
    std::cout << "Render " << _scene.name() << " done in: " << passedTime / 1000 << "sec(s)" << std::endl;

    timeStart = std::chrono::high_resolution_clock::now();

    // Pixels rendered so write to file
    PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
    file.writeFrameBuffer(framebuffer);
	
    timeEnd = std::chrono::high_resolution_clock::now();
    passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
    std::cout << "Write " << filename << " done in: " << passedTime / 1000 << "sec(s)" << std::endl;
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

Color Renderer::castRay(const Ray& ray) const
{
    // Trace primary ray
    Intersaction intersaction;
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

Vector Renderer::primaryRayDirection(int pixelIdx) const
{
    const Settings& settings = _scene.settings();

    // Convert index based pixel to x, y coordinates
    float x = pixelIdx % settings.ImageWidth;
    float y = pixelIdx / settings.ImageWidth;

    const Camera& camera = _scene.camera();    
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

