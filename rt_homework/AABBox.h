#pragma once

#include "Utils.h"
#include "Ray.h"

class AABBox
{
public:
    AABBox();

    bool checkIntersection(const Ray& ray) const;
    bool checkIntersection(const AABBox& box) const;
    void expandBox(const Triangle& triangle);
    void reset();
    bool split(int axisIndx, AABBox& box1, AABBox& box2) const;

private:
    Vector _min;
    Vector _max;
};

