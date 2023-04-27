#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"
#include "Ray.h"
#include <chrono>

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
    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	
    // Pixels rendered so write to file
    PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
    file.writeFrameBuffer(framebuffer);
	
    return passedTime;
}

Vector Renderer::castRay(float x, float y) const
{
    Vector rayDirection(x, y, -1);
    const Scene::Settings& settings = _scene.settings();
    rayDirection = rayDirection.getCenter().getNDC((float)settings.ImageWidth, (float)settings.ImageHeight).getWorldSpace();

    // Consider the aspect ratio
    float aspectRatio = (float)settings.ImageWidth / (float)settings.ImageHeight;
    rayDirection.setX(rayDirection.x() * aspectRatio);

    // Set camera rotation matrix
    const Camera& camera = _scene.camera();
    rayDirection = camera.rotation() * rayDirection;

    rayDirection = rayDirection.normalize();

    Ray ray(_scene.triangles());
    Vector hitPoint;

    // Trace camera ray
    const Triangle* triangle = ray.trace(camera.position(), rayDirection, hitPoint);

    if (triangle != nullptr) {
       Vector finalColor;
       for (const auto& light : _scene.lights()) {
            Vector lightDir = light.Position - hitPoint;
            float radius = lightDir.magnitude();
            lightDir = lightDir.normalize();
     
            // Trace shadow ray
            Vector origin = hitPoint + triangle->normal() * _shadowBias;
            ray.setShadowRay(true);
            const auto hit = ray.trace(origin, lightDir, hitPoint);
            if(hit == nullptr) {
                float area = getArea(radius);
                float cosLaw = std::max(0.f, lightDir.dot(triangle->normal()));
                float colorCorrection = light.Intensity / area * cosLaw;
                Vector lightContribution(_alberdo);
                lightContribution *= colorCorrection;
                finalColor += lightContribution;
            } else {
                int a = 0;
                a++;
            }
       }

       return finalColor;
    }

    return settings.BackGroundColor;
}

float Renderer::getArea(float radius) const
{
    return 4 * PI * radius * radius;
}
