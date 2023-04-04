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
	Vector() = default;
	Vector(float x, float y, float z);
	Vector& operator=(const Vector& other);
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

	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
};

struct Triangle {
	Triangle() = default;
	Triangle(const Vector& v0, const Vector& v1, const Vector& v2);

	Vector normal() const;
	Color color() const;
	void setColor(const Color& color);
	float distance() const { return _distance; }
	
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
};
