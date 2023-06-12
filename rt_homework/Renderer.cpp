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
    int totalMS = 0;
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

    int passedTime = (int)std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - timeStart).count();
    if (log) {
        *log << "Render " << _scene.name() << " done in: " << passedTime << " ms" << std::endl;
    }

    totalMS += passedTime;

    // Could pass empty string and render only on screen so check it
    if(!filename.empty()) {
        timeStart = std::chrono::high_resolution_clock::now();

        // Pixels were rendered, so write to file
        PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
        file.writeFrameBuffer(framebuffer);
	
        passedTime = (int)std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - timeStart).count();
        if (log) {
            *log << "Write " << filename << " done in: " << passedTime << " ms" << std::endl;
        }
        totalMS += passedTime;
    }

    return totalMS;
}

Color Renderer::castRay(const Ray& ray) const
{
    // Trace primary ray
    Intersaction intersaction;
    if (_scene.intersect(ray, &intersaction)) {
        switch (intersaction.Triangle->metrial().Type)
        {
            case Material::Type::Diffuse: 
            {
                Color finalColor;
                for (const auto& light : _scene.lights()) {
                    // Shadow ray direction
                    Vector lightDir;
                    float lightLenght = light.getIllumination(intersaction.Point, lightDir);
                    float area = Utils::getArea(lightLenght);
                    const Vector surfNormal = intersaction.Triangle->hitNormal(intersaction.Point);

                    // Trace shadow ray
                    ShadowRay shadowRay(
                        intersaction.Point + surfNormal * _scene.settings().Bias, 
                        lightDir, 
                        lightLenght,
                        ray.depth());

                    if(!_scene.intersect(shadowRay)) {
                        // Trace shadow could use the smooth normal
                        float cosLaw = std::max(0.f, lightDir.dot(surfNormal));
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

                Color color = castRay(ray);
                return color * intersaction.Triangle->metrial().Albedo;
            }

            case Material::Type::Refractive: 
            {               
                if(ray.depth() >= _scene.settings().MaxDepth) {
                    return _scene.settings().BackGroundColor;
                }
 
                Vector refrationDir;
                Color refractionColor;
                Vector surfNormal = intersaction.Triangle->hitNormal(intersaction.Point);
                bool castRefraction = refractRayDirection(ray.direction(), surfNormal, intersaction.Triangle->metrial().IOR, refrationDir);
                if(castRefraction)
                {    
                    // Cast refraction ray
                    PrimaryRay refrationRay(
                        intersaction.Point + (-surfNormal * _scene.settings().RefractionBias),
                        refrationDir, 
                        ray.depth());
                    refractionColor = castRay(refrationRay);
                }

                // Cast reflecation ray
                PrimaryRay reflectionRay(
                    intersaction.Point + surfNormal * _scene.settings().Bias, 
                    reflectiveRayDirection(ray.direction(), surfNormal), 
                    ray.depth());

                Color reflectionColor = castRay(reflectionRay);

                // Caclulate fresnel only if we have refraction ray casted
                if(castRefraction) {
                    float fresnel = Utils::fresnel(ray.direction(), surfNormal);
                    return reflectionColor * fresnel + refractionColor * (1.f - fresnel);
                }

                return reflectionColor;
            }
        
            case Material::Type::Constant:
                return intersaction.Triangle->metrial().Albedo;

            default:
                assert(false);
                break;
        }
    }
    return _scene.settings().BackGroundColor;
}


Color Renderer::reflectiveRayDirection(const Vector& rayDir, const Vector& surfNormal) const
{
    return (rayDir - (surfNormal * 2.f * rayDir.dot(surfNormal))).normal();
}

bool Renderer::refractRayDirection(const Vector& rayDir, Vector& surfNormal, const float& ior, Vector& refRayDir) const
{
    float etai = 1.f;
    float etat = ior;
    float cosi = rayDir.dot(surfNormal);
    if(cosi < 0.f) {
        cosi = -cosi;
    } else {
        surfNormal = -surfNormal;
        std::swap(etai, etat);                    
    }
 
    float eta = etai / etat;  
    float sint = eta * sqrtf(std::max(0.f, 1 - cosi * cosi));
    if (sint >= 1.f) {  // verifies for total internal reflection
        return false;
    }

    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    refRayDir = rayDir * eta + surfNormal * (eta * cosi - cost);
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

