#include "Utils.h"
#include "Camera.h"
#include <sstream>

using namespace std;
Color::Color(int r, int g, int b) :
	R(r), G(g), B(b) {
}

/*static*/ Color Color::random(int maxColorComponent)
{
	return Color(rand() % maxColorComponent, rand() % maxColorComponent, rand() % maxColorComponent);
}

string Color::toString() {
	stringstream ss;
	ss << R << " " << G << " " << B << "\t";
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
	_array[0] = other.x();
	_array[1] = other.y();
	_array[2] = other.z();
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
	return Vector(x() + other.x(), y() + other.y(), z() + other.z());
}

Vector Vector::operator*(float scalar) const
{
	return Vector(x() * scalar, y() * scalar, z() * scalar);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(x() - other.x(), y() - other.y(), z() - other.z());
}

ostream& operator<<(ostream& os, const Vector& other)
{
	 os << other.x() << ", " << other.y() << ", " << other.z();
	 return os;
}

Vector Vector::getNDC(float imageWidth, float imageHeight) const
{
	// Convert to NDC
	return Vector(x() / imageWidth, y() / imageHeight, z());
}

Vector Vector::getWorldSpace() const
{
	// Convert to World space
	return Vector((2.0f * x()) - 1.0f, 1.0f - (2.0f * y()), z());
}

Vector Vector::getCenter() const
{
	return Vector(x() + 0.5f, y() + 0.5f, z());
}

float Vector::magnitude() const 
{
	return sqrt((x() * x()) + (y() * y()) + (z() * z()));
}

Vector Vector::normalize() const
{
	float invertLength = 1.f / magnitude();
	return Vector(x() * invertLength, y() * invertLength, z() * invertLength);
}

Vector Vector::cross(const Vector& other) const
{
	float detX = (y() * other.z()) - (z() * other.y());
	float detY = ((x() * other.z()) - (z() * other.x())) * -1.f;
	float detZ = (x() * other.y()) - (y() * other.x());

	return Vector(detX, detY, detZ);
}

float Vector::dot(const Vector& other) const
{
	return (x() * other.x()) + (y() * other.y()) + (z() * other.z());
}

Color Vector::toColor(int maxColorComponent) const
{
	return Color((int)(abs(x()) * maxColorComponent), (int)(abs(y()) * maxColorComponent), (int)(abs(z()) * maxColorComponent));
}

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class


Triangle::Triangle(const Vector& v0, const Vector& v1, const Vector& v2) :
	V0(v0), V1(v1), V2(v2)
{
	recalc();
}

void Triangle::setCamera(const Camera& camera)
{
	_camera = &camera;
	_distance = _normal.dot(V0 - _camera->position());
}

void Triangle::recalc()
{
	_vE0 = V1 - V0;
	_vE1 = V2 - V1;
	_vE2 = V0 - V2;

	Vector E1 = V2 - V0;
	_normal = _vE0.cross(E1).normalize();
	if(_camera) {
		_distance = _normal.dot(V0 - _camera->position());
	} else {
		_distance = _normal.dot(V0);
	}
}

Vector Triangle::normal() const
{	
	return _normal;
}

void Triangle::setColor(const Color& color)
{
	_color = color;
}

Color Triangle::color() const
{
	return _color;
}

float Triangle::area() {
	Vector v = normal();
	return v.magnitude() / 2;
}

bool Triangle::checkIntersaction(const Vector& point) const
{
	Vector v0P = point - V0;
	// Check if P is on the left of E0
	if(_normal.dot(_vE0.cross(v0P)) <= 0)
		return false;

	Vector v1P = point - V1;
	// Check if P is on the left of E1
	if(_normal.dot(_vE1.cross(v1P)) <= 0)
		return false;
	
	Vector v2P = point - V2;	
	// Check if P is on the left of E2
	if(_normal.dot(_vE2.cross(v2P)) <= 0)
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
