#include "Triangle.h"
#include "Scene.h"

Triangle::Triangle(Vertex& v0, Vertex& v1, Vertex& v2, const Material& material) :
	V0(v0), V1(v1), V2(v2), _material(material)
{
	recalc();
}

float Triangle::distance(const Point& origin) const
{
	// Could be precalculated in later stage
	return _normal.dot(V0.Vert - origin);
}

void Triangle::normalizeVertices()
{
	V0.Normal.normalize();
	V1.Normal.normalize();
	V2.Normal.normalize();
}

void Triangle::recalc()
{
	_V0V1 = V1.Vert - V0.Vert;
	_V1V2 = V2.Vert - V1.Vert;
	_V2V0 = V0.Vert - V2.Vert;
	_V0V2 = V2.Vert - V0.Vert;

	Vector normalVec(_V0V1.cross(_V0V2));
	_rectArea = normalVec.magnitude();
	_normal = normalVec.normal();

	V0.Normal += _normal;
	V1.Normal += _normal;
	V2.Normal += _normal;
}

const Vector Triangle::hitNormal(const Point& p) const
{
	if(_material.SmoothShading) {
		return smoothNormal(p);
	} else {
		return normal();
	}
}

const Vector& Triangle::normal() const
{	
	return _normal;
}

const Vector Triangle::smoothNormal(const Point& p) const
{
	// Calculate smoothNormal
	UV uvCoord = uv(p);
	Vector smoorthNormal(V1.Normal* uvCoord.u + V2.Normal * uvCoord.v + V0.Normal * (1.f - uvCoord.u - uvCoord.v));
	return smoorthNormal.normal();
}

UV Triangle::uv(const Point& p) const
{
	Vector V0P = p - V0.Vert;

	float invArea = 1.f / _rectArea;
	

	/*
	float area = _V0V1.cross(_V0V2).magnitude();
	Vector V1P = p - V1.Vert;
	UV uv{  
		_V0V1.cross(V1P).magnitude() / area,
		_V1V2.cross(V1P).magnitude() / area
	};
	*/

	UV uv{
		V0P.cross(_V0V2).magnitude() * invArea,
		_V0V1.cross(V0P).magnitude() * invArea
	};

	// TODO check why assert
	//assert(uv.u + uv.v <= 1.f);
	return uv;

}

void Triangle::setColor(const Color& color)
{
	_color = color;
}

ColorRGB Triangle::color() const
{
	return _color.toColor(255);
}

float Triangle::area() const 
{
	// Triangle area is half of the rectangle
	return _rectArea / 2;
}

bool Triangle::checkIntersaction(const Point& point) const
{
	Vector v0P = point - V0.Vert;
	// Check if P is on the left of E0
	if(_normal.dot(_V0V1.cross(v0P)) < 0)
		return false;

	Vector v1P = point - V1.Vert;
	// Check if P is on the left of E1
	if(_normal.dot(_V1V2.cross(v1P)) < 0)
		return false;
	
	Vector v2P = point - V2.Vert;
	// Check if P is on the left of E2
	if(_normal.dot(_V2V0.cross(v2P)) < 0)
		return false;

	// Got intersaction
	return	true;
}