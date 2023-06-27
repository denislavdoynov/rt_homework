#include "AABBox.h"
#include "Triangle.h"
#include <assert.h>

AABBox::AABBox()
{
    reset();
}

void AABBox::expandBox(const Triangle& triangle) 
{
    _min.setMin(triangle.V0.Vert).setMin(triangle.V1.Vert).setMin(triangle.V2.Vert);
    _max.setMax(triangle.V0.Vert).setMax(triangle.V1.Vert).setMax(triangle.V2.Vert);
}

bool AABBox::checkIntersection(const Ray& ray) const 
{
    using namespace std;

    float tmin = MIN_FLOAT;
    float tmax = MAX_FLOAT;
    Vector invD = 1.f / ray.direction();
	Vector t0s = (_min - ray.origin()) * invD;
  	Vector t1s = (_max - ray.origin()) * invD;
    
  	Vector tsmaller = Vector::minVec(t0s, t1s);
    Vector tbigger  = Vector::maxVec(t0s, t1s);
    
    tmin = max(tmin, max(tsmaller[0], max(tsmaller[1], tsmaller[2])));
    tmax = min(tmax, min(tbigger[0], min(tbigger[1], tbigger[2])));

	return (tmin < tmax);
}

bool AABBox::checkIntersection(const AABBox& box) const
{
    for(int i = 0; i < Vector::SIZE; ++i) {
        if(_min[i] > box._max[i] || _max[i] < box._min[i])
            return false; 
    }

    return true;
}

void AABBox::reset() 
{
    _min.x = MAX_FLOAT;
    _min.y = MAX_FLOAT;
    _min.z = MAX_FLOAT;

    _max.x = MIN_FLOAT;
    _max.y = MIN_FLOAT;
    _max.z = MIN_FLOAT;
}

bool AABBox::split(int axisIndx, AABBox& box1, AABBox& box2)  const
{
    if(axisIndx >= Vector::SIZE) {
        assert(false);
        return false;
    }
      
    const float middleCoord = _min[axisIndx] + (_max[axisIndx] - _min[axisIndx]) / 2;

    box1 = *this;
    box2 = *this;
    box1._max[axisIndx] = middleCoord;
    box2._min[axisIndx] = middleCoord;
    return true;
}