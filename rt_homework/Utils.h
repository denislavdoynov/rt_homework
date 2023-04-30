#pragma once

#include <string>

const float PI = 3.14159265358979f;

constexpr float ROUNDING_ERROR_FLOAT = 0.0001f;

class Utils
{
public:
	static float getArea(float radius);
	//! returns if a equals b, taking possible rounding errors into account
	static bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT);

	static float clamp(const float& lo, const float& hi, const float& v);
};

struct ColorRGB {
	ColorRGB() = default;
	ColorRGB(unsigned short r, unsigned short g, unsigned short b);
	static ColorRGB random(int maxColorComponent);
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
	bool operator==(const Vector& other);
	Vector operator-(const Vector& other) const;
	Vector operator*(float scalar) const;
	Vector& operator*=(float scalar);
	Vector operator/(float scalar) const;
	friend std::ostream& operator<<(std::ostream& os, const Vector& other);

	Vector getNDC(float imageWidth, float imageHeight) const;
	Vector getWorldSpace() const;
	Vector getCenter() const;
	void normalize();
	Vector normal() const;
	float magnitude() const;
	Vector cross(const Vector& other) const;
	float dot(const Vector& other) const;
	ColorRGB toColor(int maxColorComponent) const;

	float x() const { return _array[0]; }
	float y() const { return _array[1]; }
	float z() const { return _array[2]; }

	void setX(float x) { _array[0] = x; }
	void setY(float y) { _array[1] = y; }
	void setZ(float z) { _array[2] = z; }

private:
	float _array[SIZE] { 0.f, 0.f, 0.f };
};

typedef Vector Point;
typedef Vector Color;

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

struct Material;

struct Triangle {
	Triangle() = delete;
	Triangle(const Vector& v0, const Vector& v1, const Vector& v2, const Material& material);
	
	const Vector& normal() const;
	const Vector& smoothNormal(const Point& p);
	ColorRGB color() const;
	void setColor(const Color& color);
	float distance(const Point& origin) const;
	const Material& metrial() const { return _material;	}
	
	float area() const;
	float area2() const;
	float getU(const Point& p) const;
	float getV(const Point& p) const;
	// Recalculate normal and edge vectors
	void recalc();
	bool checkIntersaction(const Point& point) const;

	Vector V0;
	Vector V1;
	Vector V2;

private:
	Vector _normal;
	Vector _smoothNormal;
	Vector _V0V1;
	Vector _V1V2;
	Vector _V2V0;
	Vector _V0V2;
	Color _color{ 1.f, 1.f, 1.f };
	const Material& _material;
};