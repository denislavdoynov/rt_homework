#include "Utils.h"
#include "Camera.h"
#include <sstream>
#include <algorithm>

using namespace std;
ColorRGB::ColorRGB(unsigned short r, unsigned short g, unsigned short b) :
	R(r), G(g), B(b) {
}

/*static*/ ColorRGB ColorRGB::random(int maxColorComponent)
{
	return ColorRGB(rand() % maxColorComponent, rand() % maxColorComponent, rand() % maxColorComponent);
}

string ColorRGB::toString() {
	stringstream ss;
	ss << R << " " << G << " " << B << "\t";
	return ss.str();
}

//////////////////////////////////////////////////////////////////////////////
// Vector
/////////////////////////////////////////////////////////////////////////////

inline
float clamp(const float& lo, const float& hi, const float& v)
{
	return std::max(lo, std::min(hi, v));
}

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
	if(index >= SIZE ) {
		return _array[0];
	}

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

Vector Vector::normalize() const
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
	unsigned short r = (unsigned short)(maxColorComponent * clamp(0, 1, _array[0]));
	unsigned short g = (unsigned short)(maxColorComponent * clamp(0, 1, _array[1]));
	unsigned short b = (unsigned short)(maxColorComponent * clamp(0, 1, _array[2]));
	return {r, g, b};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class


Triangle::Triangle(const Vector& v0, const Vector& v1, const Vector& v2) :
	V0(v0), V1(v1), V2(v2)
{
	recalc();
}

float Triangle::distance(const Vector& origin) const
{
	// Could be precalculated in later stage
	return _normal.dot(V0 - origin);
}

Vector Triangle::tst() const
{

	return (V1 - V0).cross(V2 - V0).normalize();
}

void Triangle::recalc()
{
	_vE0 = V1 - V0;
	_vE1 = V2 - V1;
	_vE2 = V0 - V2;

	Vector E1 = V2 - V0;
	_normal = _vE0.cross(E1).normalize();
}

Vector Triangle::normal() const
{	
	return _normal;
}

void Triangle::setColor(const Vector& color)
{
	_color = color;
}

ColorRGB Triangle::color() const
{
	return _color.toColor(255);
}

float Triangle::area() {
	Vector v = normal();
	return v.magnitude() / 2;
}

bool Triangle::checkIntersaction(const Vector& point) const
{
	Vector v0P = point - V0;
	// Check if P is on the left of E0
	if(_normal.dot(_vE0.cross(v0P)) < 0)
		return false;

	Vector v1P = point - V1;
	// Check if P is on the left of E1
	if(_normal.dot(_vE1.cross(v1P)) < 0)
		return false;
	
	Vector v2P = point - V2;	
	// Check if P is on the left of E2
	if(_normal.dot(_vE2.cross(v2P)) < 0)
		return false;

	// Got intersaction
	return	true;
}

Matrix::Matrix(float a1, float a2, float a3,
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
	Vector res;
	if(SIZE != other.SIZE)
		return res;

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			res[i] += (_array[i][j] * other[j]);
		}
	}

	return res;
}
Matrix Matrix::operator*(const Matrix& other) const
{
	Matrix res;
	if (SIZE != other.SIZE)
		return res;

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			res._array[i][j] = 0;
			for (int k = 0; k < SIZE; k++) {
				res._array[i][j] += _array[i][k] * other._array[k][j];
			}
		}
	}

	return res;
}


float Math::getArea(float radius)
{
	return 4 * PI * radius * radius;
}

//! returns if a equals b, taking possible rounding errors into account
bool Math::equals(const float a, const float b, const float tolerance)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
