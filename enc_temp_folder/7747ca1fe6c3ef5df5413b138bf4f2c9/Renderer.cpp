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
                PrimaryRay ray(_scene.camera().position(), primaryRayDirection(i));
                framebuffer[i] = castRay(ray);
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

Color Renderer::castRay(const Ray& ray) const
{
    // Trace primary ray
    Intersaction intersaction;
    if (_scene.intersect(ray, &intersaction)) {
        Color finalColor;

        switch (intersaction.Triangle->metrial().Type)
        {
            case Material::Type::Diffuse: 
            {
                for (const auto& light : _scene.lights()) {
                    // Shadow ray direction
                    Vector lightDir;
                    float area = light.getIllumination(intersaction.Point, lightDir);
     
                    // Trace shadow ray
                    Point origin(intersaction.Point + intersaction.Triangle->normal() * _scene.settings().Bias);
                    ShadowRay shadowRay(origin, lightDir, ray.depth());
                    if(!_scene.intersect(shadowRay)) {
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

            }

            case Material::Type::Reflective: 
            {        
                if(ray.depth() >= _scene.settings().MaxDepth) {
                    return _scene.settings().BackGroundColor;
                }

                // Cast reflective ray
                Vector surfNormal = intersaction.Triangle->hitNormal(intersaction.Point);
                PrimaryRay reflectionRay(
                    intersaction.Point + surfNormal * _scene.settings().Bias, 
                    reflectiveRayDirection(ray.direction(), surfNormal), 
                    ray.depth());

                finalColor = castRay(ray);
                finalColor *= intersaction.Triangle->metrial().Albedo;
                return finalColor;
            }

            case Material::Type::Refractive: 
            {               
                if(ray.depth() >= _scene.settings().MaxDepth) {
                    return _scene.settings().BackGroundColor;
                }
 
                float cosThetaI = ray.direction().dot(intersaction.Triangle->normal());
                float etaI = 1.f;
                float etaT = intersaction.Triangle->metrial().IOR;

                Vector surfNormal = intersaction.Triangle->hitNormal(intersaction.Point);
                if(cosThetaI > 0.f) {
                    surfNormal = -surfNormal;
                    std::swap(etaI, etaT);
                } else {
                    cosThetaI = -cosThetaI;
                }

                Vector refrationDir;
                Color refractionColor;
                if(refractRayDirection(ray.direction(), surfNormal, etaI / etaT, cosThetaI, refrationDir))
                {    
                    // Cast refractive ray
                    PrimaryRay refrationRay(
                        intersaction.Point + (-surfNormal * _scene.settings().RefractionBias),
                        refrationDir, 
                        ray.depth());
                    refractionColor = castRay(refrationRay);
                }

                // Cast reflective ray
                PrimaryRay reflectionRay(
                    intersaction.Point + surfNormal * _scene.settings().Bias, 
                    reflectiveRayDirection(ray.direction(), surfNormal), 
                    ray.depth());
                Color reflectionColor = castRay(reflectionRay);

                float fresnel = Utils::fresnel(ray.direction(), surfNormal);
                finalColor = reflectionColor * fresnel + refractionColor * (1 - fresnel);

                // TODO NOt sure if we apply albedo here
                //finalColor *= intersaction.Triangle->metrial().Albedo;
                return finalColor;
            }
        
            case Material::Type::Constant:
            default:
                break;
        }
        
    }
    return _scene.settings().BackGroundColor;
}

Color Renderer::reflectiveRayDirection(const Vector& rayDir, const Vector& surfNormal) const
{
    return (rayDir - (surfNormal * 2.f * rayDir.dot(surfNormal))).normal();
}

bool Renderer::refractRayDirection(const Vector& rayDir, const Vector& surfNormal, float eta, float cosThetaI, Vector& refRayDir) const
{
    const float sin2ThetaT = 1 - eta * eta * (1 - cosThetaI * cosThetaI);
    if (sin2ThetaT < std::numeric_limits<float>::epsilon())  // verifies for total internal reflection
        return false;

    const float cosThetaT = sqrtf(sin2ThetaT);
    refRayDir = rayDir * eta + surfNormal * (eta * cosThetaI - cosThetaT);
    refRayDir.normalize();

    return true;
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

