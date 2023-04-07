#pragma once

#include "Utils.h"

class Camera
{
public:
	void truck(const Vector& move);
	void pan(const float degs);

	const Matrix& rotation() const { return _rotation; }
	const Vector& position() const { return _position; }
	void setPosition(const Vector& poistion) { _position = poistion; }

private:
	Vector _position;
	Matrix _rotation;
};