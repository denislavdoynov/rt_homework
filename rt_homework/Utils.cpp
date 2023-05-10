#include "Utils.h"
#include "Camera.h"
#include <sstream>
#include <algorithm>
#include <assert.h>
#include "Scene.h"

using namespace std;
ColorRGB::ColorRGB(unsigned char r, unsigned char g, unsigned char b) :
	R(r), G(g), B(b) {
}

/*static*/ ColorRGB ColorRGB::random(int maxColorComponent)
{
	return ColorRGB(rand() % maxColorComponent, rand() % maxColorComponent, rand() % maxColorComponent);
}

string ColorRGB::toString() {
	stringstream ss;
	ss << (short)R << " " << (short)G << " " << (short)B << "\t";
	return ss.str();
}

//////////////////////////////////////////////////////////////////////////////
// Vector
/////////////////////////////////////////////////////////////////////////////

Vector::Vector(float x, float y, float z) :
	_array{ x, y, z } {
}

Vector& Vector::operator=(const Vector& other)
{
	_array[0] = other._array[0];
	_array[1] = other._array[1];
	_array[2] = other._array[2];
	return *this;
}

float& Vector::operator[](int index)
{
	return _array[index];
}

float Vector::operator[](int index) const
{
	if (index >= SIZE) {
		return _array[0];
	}

	return _array[index];
}

Vector Vector::operator+(const Vector& other) const
{
	return Vector(_array[0] + other._array[0], _array[1] + other._array[1], _array[2] + other._array[2]);
}

Vector& Vector::operator+=(const Vector& other)
{
	_array[0] += other._array[0];
	_array[1] += other._array[1];
	_array[2] += other._array[2];
	return *this;
}

Vector& Vector::operator*=(const Vector& other)
{
	_array[0] *= other._array[0];
	_array[1] *= other._array[1];
	_array[2] *= other._array[2];
	return *this;
}

bool Vector::operator==(const Vector& other)
{
	return  _array[0] == other._array[0] &&
			_array[1] == other._array[1] &&
			_array[2] == other._array[2];
}

Vector Vector::operator*(float scalar) const
{
	return Vector(_array[0] * scalar, _array[1] * scalar, _array[2] * scalar);
}
Vector& Vector::operator*=(float scalar)
{
	_array[0] *= scalar;
	_array[1] *= scalar;
	_array[2] *= scalar;
	return *this;
}

Vector Vector::operator/(float scalar) const
{
	return Vector(_array[0] / scalar, _array[1] / scalar, _array[2] / scalar);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(_array[0] - other._array[0], _array[1] - other._array[1], _array[2] - other._array[2]);
}

ostream& operator<<(ostream& os, const Vector& other)
{
	 os << other._array[0] << ", " << other._array[1] << ", " << other._array[2];
	 return os;
}

Vector Vector::getNDC(float imageWidth, float imageHeight) const
{
	// Convert to NDC
	return Vector(_array[0] / imageWidth, _array[1] / imageHeight, _array[2]);
}

Vector Vector::getWorldSpace() const
{
	// Convert to World space
	return Vector((2.0f * _array[0]) - 1.0f, 1.0f - (2.0f * _array[1]), _array[2]);
}

Vector Vector::getCenter() const
{
	return Vector(_array[0] + 0.5f, _array[1] + 0.5f, _array[2]);
}

float Vector::magnitude() const 
{
	return sqrt((_array[0] * _array[0]) + (_array[1] * _array[1]) + (_array[2] * _array[2]));
}

void Vector::normalize()
{
	float invertLength = 1.f / magnitude();
	_array[0] *= invertLength;
	_array[1] *= invertLength;
	_array[2] *= invertLength;
}

Vector Vector::normal() const
{
	float invertLength = 1.f / magnitude();
	return Vector(_array[0] * invertLength, _array[1] * invertLength, _array[2] * invertLength);
}

Vector Vector::cross(const Vector& other) const
{
	return { _array[1] * other._array[2] - _array[2] * other._array[1], 
			 (_array[0] * other._array[2] - _array[2] * other._array[0])*-1, 
			 _array[0]* other._array[1] - _array[1] * other._array[0] };
}

float Vector::dot(const Vector& other) const
{
	return (_array[0] * other._array[0]) + (_array[1] * other._array[1]) + (_array[2] * other._array[2]);
}

ColorRGB Vector::toColor(int maxColorComponent) const
{
	unsigned char r = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _array[0]));
	unsigned char g = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _array[1]));
	unsigned char b = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _array[2]));
	return {r, g, b};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class


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

Matrix::Matrix( float a1, float a2, float a3,
				float a4, float a5, float a6,
				float a7, float a8, float a9) :
	_array{ {a1, a2, a3}, 
			{a4, a5, a6},
			{a7, a8, a9}
	}
{
}

Vector Matrix::operator*(const Vector& other) const
{
	return {
		_array[0][0] * other.x() + _array[1][0] * other.y() + _array[2][0] * other.z(),
		_array[0][1] * other.x() + _array[1][1] * other.y() + _array[2][1] * other.z(),
		_array[0][2] * other.x() + _array[1][2] * other.y() + _array[2][2] * other.z(),
	};
}
Matrix Matrix::operator*(const Matrix& other) const
{
	return {
		_array[0][0] * other._array[0][0] + _array[1][0] * other._array[0][1] + _array[2][0] * _array[0][2],
		_array[0][1] * other._array[0][0] + _array[1][1] * other._array[0][1] + _array[2][1] * _array[0][2],
		_array[0][2] * other._array[0][0] + _array[1][2] * other._array[0][1] + _array[2][2] * _array[0][2],

		_array[0][0] * other._array[1][0] + _array[1][0] * other._array[1][1] + _array[2][0] * _array[1][2],
		_array[0][1] * other._array[1][0] + _array[1][1] * other._array[1][1] + _array[2][1] * _array[1][2],
		_array[0][2] * other._array[1][0] + _array[1][2] * other._array[1][1] + _array[2][2] * _array[1][2],

		_array[0][0] * other._array[2][0] + _array[1][0] * other._array[2][1] + _array[2][0] * _array[2][2],
		_array[0][1] * other._array[2][0] + _array[1][1] * other._array[2][1] + _array[2][1] * _array[2][2],
		_array[0][2] * other._array[2][0] + _array[1][2] * other._array[2][1] + _array[2][2] * _array[2][2]
	};
}


float Utils::getArea(float radius)
{
	return 4 * PI * radius * radius;
}

//! returns if a equals b, taking possible rounding errors into account
bool Utils::equals(const float a, const float b, const float tolerance)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

float Utils::clamp(const float& lo, const float& hi, const float& v)
{
	return std::max(lo, std::min(hi, v));
}