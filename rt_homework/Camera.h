#pragma once

#include "Utils.h"

class Camera
{
public:
	void truck(const Vector& move);
	void pan(const float degs);
	void tilt(const float degs);
	void roll(const float degs);

	const Matrix& rotation() const { return _rotation; }
	const Vector& position() const { return _position; }
	void setPosition(const Vector& position);

private:
	float convertToRads(const float degs);

private:
	Vector _position;
	Matrix _rotation;
};