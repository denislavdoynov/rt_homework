#include "Renderer.h"
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

    _totalElapsedTime = 0;
    _bucketManager.updateSceneSettings(_scene.settings());

    FrameBuffer framebuffer(_bucketManager.totalPixels());
    auto timeStart = std::chrono::high_resolution_clock::now();
#ifdef MULTI_THREADED
    if(log) {
        *log << "Multi-threaded render started..." << std::endl;
    }
       
    _threadManager.startRender(*this, framebuffer, _bucketManager);

    if(_abort)
        return 0;

#else
    if (log) {
        *log << "Single-threaded render started..." << std::endl;
    }

    for (uint32_t i = 0; i < _bucketManager.totalPixels(); ++i) {
        framebuffer[i] = castCameraRay(i);
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

    _totalElapsedTime += passedTime;

    // Could pass empty string and render only on screen so check it
    if(!filename.empty()) {
        // Pixels were rendered, so write to file
        passedTime = writeOutputFile(filename, framebuffer);
        if (log) {
            *log << "Write " << filename << " done in: " << passedTime << " ms" << std::endl;
        }
        _totalElapsedTime += passedTime;
    }

    return _totalElapsedTime;
}

int Renderer::writeOutputFile(const std::string& filename, const FrameBuffer& framebuffer) 
{
    auto start = std::chrono::high_resolution_clock::now();
    const Settings& settings = _scene.settings();
    PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
    file.writeFrameBuffer(framebuffer);
	
    return (int)std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();
}

Color Renderer::castCameraRay(int pixelIdx) const
{
    PrimaryRay ray(_scene.camera().position(), primaryRayDirection(pixelIdx));
    // Check for intersecation with aabbox otherwise return background color

    Triangles trianlges;
    if(_scene.intersectAABBox(ray, trianlges)) {
        return castRay(ray, trianlges);
    }
    
    return _scene.settings().BackGroundColor;
}

Color Renderer::castRay(const Ray& ray, const Triangles& triangles) const
{
    // Trace primary ray
    Intersaction intersaction;
    if (_scene.intersect(ray, triangles, &intersaction)) {
        switch (intersaction.Triangle->metrial().Type)
        {
            case Material::Type::Diffuse: 
               return shadeDeffuse(ray, intersaction);
            
            case Material::Type::Reflective: 
                return shadeReflective(ray, intersaction);

            case Material::Type::Refractive: 
                return shadeRefractive(ray, intersaction);
        
            case Material::Type::Constant:
                return intersaction.Triangle->metrial().Albedo;

            default:
                assert(false);
                break;
        }
    }
    return _scene.settings().BackGroundColor;
}

Color Renderer::shadeDeffuse(const Ray& ray, const Intersaction& intersaction) const 
{
    Color color;
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

        if(!_scene.intersect(shadowRay, _scene.triangles())) {
            // Trace shadow could use the smooth normal
            float cosLaw = std::max(0.f, lightDir.dot(surfNormal));
            float colorCorrection = light.Intensity / area * cosLaw;
            Color lightContribution(intersaction.Triangle->metrial().Albedo);
            lightContribution *= colorCorrection;
            color += lightContribution;
        }
    }

    #ifdef BARYCENTRIC_COLORS
    UV uvCoord = intersaction.Triangle->uv(intersaction.Point);
    color.setY(uvCoord.u);
    color.setZ(uvCoord.v);
    #endif

    return color;
}

Color Renderer::shadeReflective(const Ray& ray, const Intersaction& intersaction) const 
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

    Color color = castRay(ray, _scene.triangles());
    return color * intersaction.Triangle->metrial().Albedo;
}

Color Renderer::shadeRefractive(const Ray& ray, const Intersaction& intersaction) const 
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
        refractionColor = castRay(refrationRay, _scene.triangles());
    }

    // Cast reflecation ray
    PrimaryRay reflectionRay(
        intersaction.Point + surfNormal * _scene.settings().Bias, 
        reflectiveRayDirection(ray.direction(), surfNormal), 
        ray.depth());

    Color reflectionColor = castRay(reflectionRay, _scene.triangles());

    // Caclulate fresnel only if we have refraction ray casted
    if(castRefraction) {
        float fresnel = Utils::fresnel(ray.direction(), surfNormal);
        return reflectionColor * fresnel + refractionColor * (1.f - fresnel);
    }

    return reflectionColor;
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

void Renderer::abort() 
{ 
    _abort = true;
    _threadManager.abort();
}

