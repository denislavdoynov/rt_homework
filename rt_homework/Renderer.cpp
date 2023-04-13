#include "Renderer.h"
#include "Scene.h"
#include "PPMFile.h"
#include "Common.h"

Renderer::Renderer(Scene& scene) :
    _scene(scene)
{
}

void Renderer::renderScene(const std::string& filename)
{
    const Scene::Settings& settings = _scene.settings();

	PPMFile file(filename, settings.ImageWidth, settings.ImageHeight, MAX_COLOR_COMPONENT);
	for (int y = 0; y < settings.ImageHeight; ++y) {
		for (int x = 0; x < settings.ImageWidth; ++x) {
			file.stream() << traceRay((float)x, (float)y).toString();
		}
		file.stream() << std::endl;
	}
}

//! returns if a equals b, taking possible rounding errors into account
bool Renderer::equals(const float a, const float b, const float tolerance)
{
    return (a + tolerance >= b) && (a - tolerance <= b);
}

Color Renderer::traceRay(float x, float y)
{
    const Scene::Settings& settings = _scene.settings();
    const Camera& camera = _scene.camera();
    Vector rayDirection(x, y, -1);
    rayDirection = rayDirection.getCenter().getNDC((float)settings.ImageWidth, (float)settings.ImageHeight).getWorldSpace();

    // Consider the aspect ratio
    float aspectRatio = (float)settings.ImageWidth / (float)settings.ImageHeight;
    rayDirection.setX(rayDirection.x() * aspectRatio);

    // Set camera rotation matrix
    rayDirection = camera.rotation() * rayDirection;

    rayDirection = rayDirection.normalize();

    const Triangle* intersactionTriangle = nullptr;
    float pointDistance = 0.f;
    for (const auto& triangle : _scene.triangles()) {
        float normalDotRayDir = rayDirection.dot(triangle.normal());
        // If generated ray is not paralel to triangle plane
        if (!equals(normalDotRayDir, 0.0f)) {
            // If R is towards the triangle’s plane
            float distance = triangle.distance(camera.position());
            if (distance < 0) {
                float t = distance / normalDotRayDir;
                Vector p = camera.position() + (rayDirection * t);
                if (triangle.checkIntersaction(p)) {
                    // Check if point is more close to the origin
                    if (intersactionTriangle == nullptr || t < pointDistance) {
                        pointDistance = t;
                        intersactionTriangle = &triangle;
                    }
                }
            }

        }
    }

    if (intersactionTriangle != nullptr) {
        return intersactionTriangle->color();
    }

    return settings.BackGroundColor.toColor(MAX_COLOR_COMPONENT);
}