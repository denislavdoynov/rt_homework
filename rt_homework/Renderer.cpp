#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"
#include "Ray.h"
#include "FrameBuffer.h"

#include <chrono>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <memory>

#define MULTI_THREADED
//#define BARYCENTRIC_COLORS

Renderer::Renderer(const Scene& scene) :
    _scene(scene)
{
}

Renderer::Renderer(std::shared_ptr<Scene>& scene) :
    _scenePtr(scene), _scene(*_scenePtr)
{
}

int Renderer::renderScene(const std::string& filename, FrameBuffer* buffer, std::stringstream* log)
{
    if (filename.empty() && !buffer) {
        if (log) {
            *log << "No output for rendering. Giving up..." << std::endl;
        }
        return 0;
    }

    auto timeStart = std::chrono::high_resolution_clock::now();
    int totalSeconds = 0;
    const Settings& settings = _scene.settings();
    FrameBuffer framebuffer(settings.ImageWidth * settings.ImageHeight);

#ifdef MULTI_THREADED
    if(log) {
        *log << "Multi-threaded render started..." << std::endl;
    }

    // Incease thread count on CPUs with more cores
    const int threadCount = 10;
    std::vector<std::thread> renderThreads;
    renderThreads.reserve(threadCount);
    for (int threadId = 0; threadId < threadCount; ++threadId) {
        renderThreads.emplace_back([this, threadId, threadCount, &framebuffer]() {
            int step = (int)framebuffer.size() / threadCount;
            int startIndex = step * threadId;
            int endIndex = (threadId+1 == threadCount) ? (int)framebuffer.size() : (startIndex + step - 1);
            for (int i = startIndex; i < endIndex; ++i) {
                int depth = 0;
                PrimaryRay ray(_scene.camera().position(), primaryRayDirection(i));
                framebuffer[i] = castRay(ray, depth);
            }
        });
    }

    // Wait for all threads to finish
    for (auto& th : renderThreads) {
        th.join();
    }

#else
    if (log) {
        *log << "Single-threaded render started..." << std::endl;
    }

    for (int i = 0; i < pixelSize; ++i) {
        int depth = 0;
        Ray ray(_scene.camera().position(), primaryRayDirection(i));
        framebuffer[i] = castRay(ray, depth);
    }
#endif

    // Used to display on the screen
    if(buffer) {
        *buffer = framebuffer;
        buffer->genImageData();
    }

    int passedTime = (int)std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - timeStart).count() / 1000;
    if (log) {
        *log << "Render " << _scene.name() << " done in: " << passedTime << " sec(s)" << std::endl;
    }

    totalSeconds += passedTime;

    // Could pass empty string and render only on screen so check it
    if(!filename.empty()) {
        timeStart = std::chrono::high_resolution_clock::now();

        // Pixels were rendered, so write to file
        PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
        file.writeFrameBuffer(framebuffer);
	
        passedTime = (int)std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - timeStart).count() / 1000;
        if (log) {
            *log << "Write " << filename << " done in: " << passedTime << " sec(s)" << std::endl;
        }
        totalSeconds += passedTime;
    }

    return totalSeconds;
}

bool Renderer::trace(const Ray& ray, Intersaction* intersaction) const
{
    if((!intersaction && !ray.shadow()) || (intersaction && ray.shadow())) {
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
                    if (ray.shadow()) {
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

Color Renderer::castRay(const Ray& ray, int& depth) const
{
    ++depth;

    // Trace primary ray
    Intersaction intersaction;
    if (trace(ray, &intersaction)) {
        Color finalColor;
        if (intersaction.Triangle->metrial().Type == Material::Type::Diffuse) {
            for (const auto& light : _scene.lights()) {
                // Shadow ray direction
                Vector lightDir;
                float area = light.getIllumination(intersaction.Point, lightDir);
     
                // Trace shadow ray
                Point origin(intersaction.Point + intersaction.Triangle->normal() * _scene.settings().ShadowBias);
                ShadowRay shadowRay(origin, lightDir);
                if(!trace(shadowRay)) {
                    // Trace shadow could use the smooth normal
                    const Vector hitNormal = intersaction.Triangle->hitNormal(intersaction.Point);
                    float cosLaw = std::max(0.f, lightDir.dot(hitNormal));
                    float colorCorrection = light.Intensity / area * cosLaw;
                    Color lightContribution(intersaction.Triangle->metrial().Albedo);
                    lightContribution *= colorCorrection;
                    finalColor += lightContribution;
                }
            }

    #ifdef BARYCENTRIC_COLORS
            UV uvCoord = intersaction.Triangle->uv(intersaction.Point);
            finalColor.setY(uvCoord.u);
            finalColor.setZ(uvCoord.v);
    #endif

            return finalColor;

        } else if (intersaction.Triangle->metrial().Type == Material::Type::Reflective) {
                       
            if(depth >= _scene.settings().ReflectiveDepth) {
                return _scene.settings().BackGroundColor;
            }
            const Vector hitNormal = intersaction.Triangle->hitNormal(intersaction.Point);
            float dotProd = ray.direction().dot(hitNormal);
            Vector reflectionDir(ray.direction() - (hitNormal * 2.f * dotProd));

            Point origin(intersaction.Point + hitNormal * _scene.settings().ShadowBias);
            PrimaryRay reflectionRay(origin, reflectionDir.normal());

            finalColor = castRay(reflectionRay, depth);
            finalColor *= intersaction.Triangle->metrial().Albedo;
            return finalColor;
        }
    }

    return _scene.settings().BackGroundColor;
}

Vector Renderer::primaryRayDirection(int pixelIdx) const
{
    const Settings& settings = _scene.settings();

    // Convert index based pixel to x, y coordinates
    float x = (float)(pixelIdx % settings.ImageWidth);
    float y = (float)(pixelIdx / settings.ImageWidth);

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

