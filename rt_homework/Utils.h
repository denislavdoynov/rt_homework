#pragma once

#include <string>
#include <iostream>

const float PI = 3.14159265358979f;

struct Color {
	Color() = default;
	Color(unsigned short r, unsigned short g, unsigned short b);
	static Color random(int maxColorComponent);
	std::string toString();

	unsigned short R = 0;
	unsigned short G = 0;
	unsigned short B = 0;
};

struct Vector {
	static constexpr size_t SIZE = 3;

	Vector() = default;
	Vector(float x, float y, float z);
	Vector& operator=(const Vector& other);
	float operator[](int index) const;
	float& operator[](int index);
	Vector operator+(const Vector& other) const;
	Vector& operator+=(const Vector& other);
	Vector operator-(const Vector& other) const;
	Vector operator*(float scalar) const;
	Vector& operator*=(float scalar);
	Vector operator/(float scalar) const;
	friend std::ostream& operator<<(std::ostream& os, const Vector& other);

	Vector getNDC(float imageWidth, float imageHeight) const;
	Vector getWorldSpace() const;
	Vector getCenter() const;
	Vector normalize() const;
	float magnitude() const;
	Vector cross(const Vector& other) const;
	float dot(const Vector& other) const;
	bool isZero() { return x() == 0 && y() == 0 && z() == 0; }
	//Vector cross(float x, float y, float z) const;
	Color toColor(int maxColorComponent) const;

	float x() const { return _array[0]; }
	float y() const { return _array[1]; }
	float z() const { return _array[2]; }

	void setX(float x) { _array[0] = x; }
	void setY(float y) { _array[1] = y; }
	void setZ(float z) { _array[2] = z; }

private:
	float _array[SIZE] { 0.f, 0.f, 0.f };
};


class Matrix
{
	static constexpr int SIZE = 3;

public:
	Matrix() = default;
	Matrix(float a1, float a2, float a3,
		float a4, float a5, float a6,
		float a7, float a8, float a9);

	Vector operator*(const Vector& other) const;
	Matrix operator*(const Matrix& other) const;

private:
	float _array[SIZE][SIZE]{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f} };
};

class Camera;

struct Triangle {
	Triangle() = default;
	Triangle(const Vector& v0, const Vector& v1, const Vector& v2);
	
	Vector tst() const;

	Vector normal() const;
	Color color() const;
	void setColor(const Vector& color);
	float distance(const Vector& origin) const;
	
	float area();
	// Recalculate normal and edge vectors
	void recalc();
	bool checkIntersaction(const Vector& point) const;

	Vector V0;
	Vector V1;
	Vector V2;

private:
	Vector _normal;
	Vector _vE0;
	Vector _vE1;
	Vector _vE2;
	Vector _color{ 1.f, 1.f, 1.f };
};