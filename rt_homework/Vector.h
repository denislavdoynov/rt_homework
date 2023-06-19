#pragma once

#include <string>

struct ColorRGB {
	ColorRGB() = default;
	ColorRGB(unsigned char r, unsigned char g, unsigned char b);
	static ColorRGB random(int maxColorComponent);
	std::string toString();

	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;
};

class Vector 
{

public:
	static constexpr size_t SIZE = 3;

	Vector() = default;
	Vector(float x, float y, float z);
	Vector& operator=(const Vector& other);
	float operator[](int index) const;
	float& operator[](int index);
	Vector operator+(const Vector& other) const;
	Vector operator/(float scalar) const;
	Vector& operator+=(const Vector& other);
	bool operator==(const Vector& other);
	bool operator<(const Vector& other);
	Vector operator-(const Vector& other) const;
	Vector operator*(float scalar) const;
	Vector operator*(const Vector& other) const;
	Vector& operator*=(const Vector& other);
	Vector& operator*=(float scalar);
	Vector operator-() const;
	friend std::ostream& operator<<(std::ostream& os, const Vector& other);
	friend Vector operator/(float scalar, const Vector& other);
	Vector getNDC(float imageWidth, float imageHeight) const;
	Vector getWorldSpace() const;
	Vector getCenter() const;
	void normalize();
	Vector normal() const;
	float magnitude() const;
	Vector cross(const Vector& other) const;
	float dot(const Vector& other) const;
	ColorRGB toColor(int maxColorComponent) const;

	Vector& setMin(const Vector& other);
	Vector& setMax(const Vector& other);

	float x() const { return _array[0]; }
	float y() const { return _array[1]; }
	float z() const { return _array[2]; }

	void setX(float x) { _array[0] = x; }
	void setY(float y) { _array[1] = y; }
	void setZ(float z) { _array[2] = z; }

	static Vector minVec(const Vector& v1, const Vector& v2);
	static Vector maxVec(const Vector& v1, const Vector& v2);

private:
	float _array[SIZE] { 0.f, 0.f, 0.f };
};

class Matrix
{
	static constexpr int SIZE = 3;

public:
	Matrix() = default;
	Matrix( float a1, float a2, float a3,
			float a4, float a5, float a6,
			float a7, float a8, float a9);

	Vector operator*(const Vector& other) const;
	Matrix operator*(const Matrix& other) const;

private:
	float _array[SIZE][SIZE]{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f} };
};

typedef Vector Point;
typedef Vector Color;

