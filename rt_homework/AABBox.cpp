#include "AABBox.h"
#include "Triangle.h"

AABBox::AABBox()
{
    reset();
}

void AABBox::expandBox(const Triangle& triangle) 
{
    _minBox.setMin(triangle.V0.Vert).setMin(triangle.V1.Vert).setMin(triangle.V2.Vert);
    _maxBox.setMax(triangle.V0.Vert).setMax(triangle.V1.Vert).setMax(triangle.V2.Vert);
}

bool AABBox::checkIntersection(const Ray& ray) const 
{
    using namespace std;

    float tmin = MIN_FLOAT;
    float tmax = MAX_FLOAT;
    Vector invD = 1.f / ray.direction();
	Vector t0s = (_minBox - ray.origin()) * invD;
  	Vector t1s = (_maxBox - ray.origin()) * invD;
    
  	Vector tsmaller = Vector::minVec(t0s, t1s);
    Vector tbigger  = Vector::maxVec(t0s, t1s);
    
    tmin = max(tmin, max(tsmaller[0], max(tsmaller[1], tsmaller[2])));
    tmax = min(tmax, min(tbigger[0], min(tbigger[1], tbigger[2])));

	return (tmin < tmax);
}

void AABBox::reset() 
{
    _minBox.setX(MAX_FLOAT);
    _minBox.setY(MAX_FLOAT);
    _minBox.setZ(MAX_FLOAT);

    _maxBox.setX(MIN_FLOAT);
    _maxBox.setY(MIN_FLOAT);
    _maxBox.setZ(MIN_FLOAT);
}
