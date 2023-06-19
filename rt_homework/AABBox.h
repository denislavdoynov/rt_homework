#pragma once

#include "Utils.h"
#include "Ray.h"

class AABBox
{
public:
    AABBox();

    bool checkIntersection(const Ray& ray) const;
    void expandBox(const Triangle& triangle);
    void reset();

private:
    Vector _minBox;
    Vector _maxBox;
};

