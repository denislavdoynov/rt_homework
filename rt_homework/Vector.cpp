#include "Vector.h"
#include "Utils.h"
#include <sstream>
#include <assert.h>

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
	data{ x, y, z } {
}

Vector& Vector::operator=(const Vector& other)
{
	data[0] = other.data[0];
	data[1] = other.data[1];
	data[2] = other.data[2];
	return *this;
}

float& Vector::operator[](int index)
{
	if (index >= SIZE) {
		assert(false);
		return data[0];
	}

	return data[index];
}

float Vector::operator[](int index) const
{
	if (index >= SIZE) {
		assert(false);
		return data[0];
	}

	return data[index];
}

Vector Vector::operator+(const Vector& other) const
{
	return {
		data[0] + other.data[0], 
		data[1] + other.data[1], 
		data[2] + other.data[2]
	};
}

Vector operator/(float scalar, const Vector& other)
{
	return {
		scalar / other[0],
		scalar / other[1],
		scalar / other[2],
	};
}

Vector& Vector::operator+=(const Vector& other)
{
	data[0] += other.data[0];
	data[1] += other.data[1];
	data[2] += other.data[2];
	return *this;
}

Vector& Vector::operator*=(const Vector& other)
{
	data[0] *= other.data[0];
	data[1] *= other.data[1];
	data[2] *= other.data[2];
	return *this;
}

Vector Vector::operator-() const 
{ 
	return {-data[0], 
			-data[1], 
			-data[2]}; 
}

bool Vector::operator==(const Vector& other)
{
	return  data[0] == other.data[0] &&
			data[1] == other.data[1] &&
			data[2] == other.data[2];
}

bool Vector::operator<(const Vector& other) 
{
	return  data[0] < other.data[0] &&
			data[1] < other.data[1] &&
			data[2] < other.data[2];
}

Vector Vector::operator*(float scalar) const
{
	return {data[0] * scalar, 
			data[1] * scalar, 
			data[2] * scalar};
}

Vector Vector::operator*(const Vector& other) const
{
	return {data[0] * other.data[0], 
			data[1] * other.data[1], 
			data[2] * other.data[2]};
}

Vector& Vector::operator*=(float scalar)
{
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;
	return *this;
}

Vector Vector::operator/(float scalar) const
{
	return Vector(data[0] / scalar, data[1] / scalar, data[2] / scalar);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(data[0] - other.data[0], data[1] - other.data[1], data[2] - other.data[2]);
}

ostream& operator<<(ostream& os, const Vector& other)
{
	 os << other.data[0] << ", " << other.data[1] << ", " << other.data[2];
	 return os;
}

Vector Vector::getNDC(float imageWidth, float imageHeight) const
{
	// Convert to NDC
	return Vector(data[0] / imageWidth, data[1] / imageHeight, data[2]);
}

Vector Vector::getWorldSpace() const
{
	// Convert to World space
	return Vector((2.0f * data[0]) - 1.0f, 1.0f - (2.0f * data[1]), data[2]);
}

Vector Vector::getCenter() const
{
	return Vector(data[0] + 0.5f, data[1] + 0.5f, data[2]);
}

float Vector::magnitude() const 
{
	return sqrt((data[0] * data[0]) + (data[1] * data[1]) + (data[2] * data[2]));
}

void Vector::normalize()
{
	float invertLength = 1.f / magnitude();
	data[0] *= invertLength;
	data[1] *= invertLength;
	data[2] *= invertLength;
}

Vector Vector::normal() const
{
	float invertLength = 1.f / magnitude();
	return Vector(data[0] * invertLength, data[1] * invertLength, data[2] * invertLength);
}

Vector Vector::cross(const Vector& other) const
{
	return { data[1] * other.data[2] - data[2] * other.data[1], 
			 (data[0] * other.data[2] - data[2] * other.data[0])*-1, 
			 data[0]* other.data[1] - data[1] * other.data[0] };
}

float Vector::dot(const Vector& other) const
{
	return (data[0] * other.data[0]) + (data[1] * other.data[1]) + (data[2] * other.data[2]);
}

ColorRGB Vector::toColor(int maxColorComponent) const
{
	unsigned char r = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, data[0]));
	unsigned char g = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, data[1]));
	unsigned char b = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, data[2]));
	return {r, g, b};
}

Vector& Vector::setMin(const Vector& other) 
{
	data[0] = min(data[0], other.data[0]); 
	data[1] = min(data[1], other.data[1]);
	data[2] = min(data[2], other.data[2]);
	return *this;
}

Vector& Vector::setMax(const Vector& other) 
{
	data[0] = max(data[0], other.data[0]); 
	data[1] = max(data[1], other.data[1]);
	data[2] = max(data[2], other.data[2]);
	return *this;
}

Vector Vector::minVec(const Vector& v1, const Vector& v2)
{
	return {
		min(v1.data[0], v2.data[0]),
		min(v1.data[1], v2.data[1]),
		min(v1.data[2], v2.data[2]),
	};
}

Vector Vector::maxVec(const Vector& v1, const Vector& v2) 
{
	return {
		max(v1.data[0], v2.data[0]),
		max(v1.data[1], v2.data[1]),
		max(v1.data[2], v2.data[2]),
	};
}

//////////////////////////////////////////////////////////////////////////////
// Matrix

Matrix::Matrix( float a1, float a2, float a3,
				float a4, float a5, float a6,
				float a7, float a8, float a9) :
	data{ {a1, a2, a3}, 
		  {a4, a5, a6},
		  {a7, a8, a9}
	}
{
}

Vector Matrix::operator*(const Vector& other) const
{
	return {
		data[0][0] * other.x + data[1][0] * other.y + data[2][0] * other.z,
		data[0][1] * other.x + data[1][1] * other.y + data[2][1] * other.z,
		data[0][2] * other.x + data[1][2] * other.y + data[2][2] * other.z,
	};
}

Matrix& Matrix::operator*=(const Matrix& other)
{
    for (int i = 0; i < Vector::SIZE; ++i) {
        for (int j = 0; j < Vector::SIZE; ++j) {
            data[i][j] =
                data[i][0] * other.data[0][j] + data[i][1] * other.data[1][j] + data[i][2] * other.data[2][j];
        }
    }
    return *this;
}