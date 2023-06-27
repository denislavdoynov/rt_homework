#include "Camera.h"
#include "Utils.h"

float Camera::convertToRads(const float degs)
{
    return degs * (PI / 180.f);
}

// TODO to be updated
void Camera::setPosition(const Vector& position)
{
    _position = position;
}

void Camera::setMatrix(const Matrix& rotMatrix)
{
    _rotation = rotMatrix;
}

void Camera::truck(float f) {
    _position += _rotation * Vector(f, 0.f, 0.f);
}

void Camera::dolly(float f) {
    _position += _rotation * Vector(0.f, 0.f, -f);
}

void Camera::boom(float f) {
    _position += _rotation * Vector(0.f, f, 0.f);
}

void Camera::pan(const float degs)
{
    const float rads = convertToRads(degs);
    const float cost = cosf(rads);
    const float sint = sinf(rads);
    const Matrix rotate{
        cost, 0.f, sint,
        0.f,  1.f, 0.f,
        -sint, 0.f, cost
    };

    _rotation *= rotate;
}

void Camera::tilt(const float degs)
{
    const float rads = convertToRads(degs);
    const float cost = cosf(rads);
    const float sint = sinf(rads);
    const Matrix rotate{
        1.f,    0.f,  0.f,
        0.f,    cost, -sint,
        0.f,    sint, cost
    };

    _rotation *= rotate;
}

void Camera::roll(const float degs)
{
    const float rads = convertToRads(degs);
    const float cost = cosf(rads);
    const float sint = sinf(rads);
    const Matrix rotate{
        cost,  -sint, 0.f,
        sint,  cost,  0.f,
        0.f,   0.f,   1.f
    };

    _rotation *= rotate;
}