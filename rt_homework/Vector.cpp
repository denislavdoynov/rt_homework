#include "Vector.h"
#include "Utils.h"
#include <sstream>

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
	_data{ x, y, z } {
}

Vector& Vector::operator=(const Vector& other)
{
	_data[0] = other._data[0];
	_data[1] = other._data[1];
	_data[2] = other._data[2];
	return *this;
}

float& Vector::operator[](int index)
{
	return _data[index];
}

float Vector::operator[](int index) const
{
	if (index >= SIZE) {
		return _data[0];
	}

	return _data[index];
}

Vector Vector::operator+(const Vector& other) const
{
	return {
		_data[0] + other._data[0], 
		_data[1] + other._data[1], 
		_data[2] + other._data[2]
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
	_data[0] += other._data[0];
	_data[1] += other._data[1];
	_data[2] += other._data[2];
	return *this;
}

Vector& Vector::operator*=(const Vector& other)
{
	_data[0] *= other._data[0];
	_data[1] *= other._data[1];
	_data[2] *= other._data[2];
	return *this;
}

Vector Vector::operator-() const 
{ 
	return {-_data[0], 
			-_data[1], 
			-_data[2]}; 
}

bool Vector::operator==(const Vector& other)
{
	return  _data[0] == other._data[0] &&
			_data[1] == other._data[1] &&
			_data[2] == other._data[2];
}

bool Vector::operator<(const Vector& other) 
{
	return  _data[0] < other._data[0] &&
			_data[1] < other._data[1] &&
			_data[2] < other._data[2];
}

Vector Vector::operator*(float scalar) const
{
	return {_data[0] * scalar, 
			_data[1] * scalar, 
			_data[2] * scalar};
}

Vector Vector::operator*(const Vector& other) const
{
	return {_data[0] * other._data[0], 
			_data[1] * other._data[1], 
			_data[2] * other._data[2]};
}

Vector& Vector::operator*=(float scalar)
{
	_data[0] *= scalar;
	_data[1] *= scalar;
	_data[2] *= scalar;
	return *this;
}

Vector Vector::operator/(float scalar) const
{
	return Vector(_data[0] / scalar, _data[1] / scalar, _data[2] / scalar);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(_data[0] - other._data[0], _data[1] - other._data[1], _data[2] - other._data[2]);
}

ostream& operator<<(ostream& os, const Vector& other)
{
	 os << other._data[0] << ", " << other._data[1] << ", " << other._data[2];
	 return os;
}

Vector Vector::getNDC(float imageWidth, float imageHeight) const
{
	// Convert to NDC
	return Vector(_data[0] / imageWidth, _data[1] / imageHeight, _data[2]);
}

Vector Vector::getWorldSpace() const
{
	// Convert to World space
	return Vector((2.0f * _data[0]) - 1.0f, 1.0f - (2.0f * _data[1]), _data[2]);
}

Vector Vector::getCenter() const
{
	return Vector(_data[0] + 0.5f, _data[1] + 0.5f, _data[2]);
}

float Vector::magnitude() const 
{
	return sqrt((_data[0] * _data[0]) + (_data[1] * _data[1]) + (_data[2] * _data[2]));
}

void Vector::normalize()
{
	float invertLength = 1.f / magnitude();
	_data[0] *= invertLength;
	_data[1] *= invertLength;
	_data[2] *= invertLength;
}

Vector Vector::normal() const
{
	float invertLength = 1.f / magnitude();
	return Vector(_data[0] * invertLength, _data[1] * invertLength, _data[2] * invertLength);
}

Vector Vector::cross(const Vector& other) const
{
	return { _data[1] * other._data[2] - _data[2] * other._data[1], 
			 (_data[0] * other._data[2] - _data[2] * other._data[0])*-1, 
			 _data[0]* other._data[1] - _data[1] * other._data[0] };
}

float Vector::dot(const Vector& other) const
{
	return (_data[0] * other._data[0]) + (_data[1] * other._data[1]) + (_data[2] * other._data[2]);
}

ColorRGB Vector::toColor(int maxColorComponent) const
{
	unsigned char r = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _data[0]));
	unsigned char g = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _data[1]));
	unsigned char b = (unsigned char)(maxColorComponent * Utils::clamp(0, 1, _data[2]));
	return {r, g, b};
}

Vector& Vector::setMin(const Vector& other) 
{
	_data[0] = min(_data[0], other._data[0]); 
	_data[1] = min(_data[1], other._data[1]);
	_data[2] = min(_data[2], other._data[2]);
	return *this;
}

Vector& Vector::setMax(const Vector& other) 
{
	_data[0] = max(_data[0], other._data[0]); 
	_data[1] = max(_data[1], other._data[1]);
	_data[2] = max(_data[2], other._data[2]);
	return *this;
}

Vector Vector::minVec(const Vector& v1, const Vector& v2)
{
	return {
		min(v1._data[0], v2._data[0]),
		min(v1._data[1], v2._data[1]),
		min(v1._data[2], v2._data[2]),
	};
}

Vector Vector::maxVec(const Vector& v1, const Vector& v2) 
{
	return {
		max(v1._data[0], v2._data[0]),
		max(v1._data[1], v2._data[1]),
		max(v1._data[2], v2._data[2]),
	};
}

//////////////////////////////////////////////////////////////////////////////
// Matrix

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