#include "Camera.h"

const float PI = 3.14159265358979f;

void Camera::truck(const Vector& move)
{
    auto moveinWorldSpace = _rotation * move;
    _position = _position + moveinWorldSpace;
}

void Camera::pan(const float degs)
{
    const float rads = degs * (PI / 180.f);
    const Matrix rotateAroundY{
        cosf(rads), 0.f, -sinf(rads),
        0.f,           1.f, -0.f,
        sinf(rads), 0.f, cosf(rads)
    };

    _rotation = _rotation * rotateAroundY;
}