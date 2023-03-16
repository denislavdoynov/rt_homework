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

Vector Vector::normalize() const
{
	float length = sqrt((X * X) + (Y * Y) + (Z * Z));
	return Vector(X / length, Y / length, Z / length);
}

Color Vector::toColor(int maxColorComponent) const
{
	return Color((int)(abs(X) * maxColorComponent), (int)(abs(Y) * maxColorComponent), (int)(abs(Z) * maxColorComponent));
}