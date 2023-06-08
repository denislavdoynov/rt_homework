#include "Ray.h"
#include "Utils.h"

Ray::Ray(const Vector& origin, const Vector& direction, int depth) :
    _origin(origin), _direction(direction), _depth(++depth)
{
}