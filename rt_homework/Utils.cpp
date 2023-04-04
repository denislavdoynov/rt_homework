#include "Utils.h"
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
	X(x), Y(y), Z(z) {
}

Vector& Vector::operator=(const Vector& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	return *this;
}


Vector Vector::operator+(const Vector& other) const
{
	return Vector(X + other.X, Y + other.Y, Z + other.Z);
}

Vector Vector::operator*(float scalar) const
{
	return Vector(X * scalar, Y * scalar, Z * scalar);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(X - other.X, Y - other.Y, Z - other.Z);
}

ostream& operator<<(ostream& os, const Vector& other)
{
	 os << other.X << ", " << other.Y << ", " << other.Z;
	 return os;
}

Vector Vector::getNDC(float imageWidth, float imageHeight) const
{
	// Convert to NDC
	return Vector(X / imageWidth, Y / imageHeight, Z);
}

Vector Vector::getWorldSpace() const
{
	// Convert to World space
	return Vector((2.0f * X) - 1.0f, 1.0f - (2.0f * Y), Z);
}

Vector Vector::getCenter() const
{
	return Vector(X + 0.5f, Y + 0.5f, Z);
}

float Vector::magnitude() const 
{
	return sqrt((X * X) + (Y * Y) + (Z * Z));
}

Vector Vector::normalize() const
{
	float invertLength = 1.f / magnitude();
	return Vector(X * invertLength, Y * invertLength, Z * invertLength);
}

Vector Vector::cross(const Vector& other) const
{
	float detX = (Y * other.Z) - (Z * other.Y);
	float detY = ((X * other.Z) - (Z * other.X)) * -1.f;
	float detZ = (X * other.Y) - (Y * other.X);

	return Vector(detX, detY, detZ);
}

float Vector::dot(const Vector& other) const
{
	return (X * other.X) + (Y * other.Y) + (Z * other.Z);
}

Color Vector::toColor(int maxColorComponent) const
{
	return Color((int)(abs(X) * maxColorComponent), (int)(abs(Y) * maxColorComponent), (int)(abs(Z) * maxColorComponent));
}

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class


Triangle::Triangle(const Vector& v0, const Vector& v1, const Vector& v2) :
	V0(v0), V1(v1), V2(v2)
{
	recalc();
}

void Triangle::recalc()
{
	_vE0 = V1 - V0;
	_vE1 = V2 - V1;
	_vE2 = V0 - V2;

	Vector E1 = V2 - V0;
	_normal = _vE0.cross(E1).normalize();
	_distance = _normal.dot(V0);
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
