#pragma once

#include <string>

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
	Vector(float x, float y, float z);
	Vector& operator=(const Vector& other);

	Vector getNDC(float imageWidth, float imageHeight) const;
	Vector getWorldSpace() const;
	Vector getCenter() const;
	Vector normalize() const;
	Color toColor(int maxColorComponent) const;

	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
};
