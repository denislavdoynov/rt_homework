#pragma once

#include <string>
#include <iostream>

struct Color {
	Color() = default;
	Color(int r, int g, int b);
	static Color random(int maxColorComponent);
	std::string toString();

	int R = 0;
	int G = 0;
	int B = 0;
};

struct Vector {
	static constexpr size_t SIZE = 3;

	Vector() = default;
	Vector(float x, float y, float z);
	Vector& operator=(const Vector& other);
	float operator[](int index) const;
	float& operator[](int index);
	Vector operator+(const Vector& other) const;
	Vector operator-(const Vector& other) const;
	Vector operator*(float scalar) const;
	friend std::ostream& operator<<(std::ostream& os, const Vector& other);

	Vector getNDC(float imageWidth, float imageHeight) const;
	Vector getWorldSpace() const;
	Vector getCenter() const;
	Vector normalize() const;
	float magnitude() const;
	Vector cross(const Vector& other) const;
	float dot(const Vector& other) const;
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

struct Triangle {
	Triangle() = default;
	Triangle(const Vector& v0, const Vector& v1, const Vector& v2);

	Vector normal() const;
	Color color() const;
	void setColor(const Color& color);
	float distance() const { return _distance; }
	void setOrigin(const Vector& origin);
	
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
	float _distance = 0.f;
	Color _color{ 255, 255, 255 };
	Vector _origin{ 0, 0, 0 };
};

class Matrix
{
	static constexpr int SIZE = 3;

public:
	Matrix() = default;
	Matrix(	float a1, float a2, float a3, 
			float a4, float a5, float a6, 
			float a7, float a8, float a9);

	Vector operator*(const Vector& other) const;
	Matrix operator*(const Matrix& other) const;
	
private:
	float _array[SIZE][SIZE]{ {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };
};