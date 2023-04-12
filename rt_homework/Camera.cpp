#include "Camera.h"

const float PI = 3.14159265358979f;

float Camera::convertToRads(const float degs)
{
    return degs * (PI / 180.f);
}

// TODO to be updated
void Camera::setPosition(const Vector& position)
{
    _position = position;
}

void Camera::truck(const Vector& move)
{
    auto moveinWorldSpace = _rotation * move;
    _position = _position + moveinWorldSpace;
}

void Camera::pan(const float degs)
{
    auto rads = convertToRads(degs);
    const Matrix rotate{
        cosf(rads), 0.f, -sinf(rads),
        0.f,           1.f, -0.f,
        sinf(rads), 0.f, cosf(rads)
    };

    _rotation = _rotation * rotate;
}

void Camera::tilt(const float degs)
{
    auto rads = convertToRads(degs);
    const Matrix rotate{
        1.f,    0.f,           0.f,
        0.f,    cosf(rads), -sinf(rads),
        0.f,    sinf(rads), cosf(rads)
    };

    _rotation = _rotation * rotate;
}

void Camera::roll(const float degs)
{
    auto rads = convertToRads(degs);
    const Matrix rotate{
        cosf(rads),  -sinf(rads), 0.f,
        sinf(rads),  cosf(rads),  0.f,
        0.f,            0.f,             1.f
    };

    _rotation = _rotation * rotate;
}