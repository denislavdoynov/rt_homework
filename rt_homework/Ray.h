#pragma once

#include "Utils.h"

class Ray
{
public:
    Ray(const Vector& origin, const Vector& direction);

    const Vector& origin() const { return _origin; }
    const Vector& direction() const { return _direction; }

private:
    const Vector _origin;
    const Vector _direction;
};

