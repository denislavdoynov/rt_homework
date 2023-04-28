#pragma once

#include "Utils.h"

class Camera
{
public:
	static constexpr const char* JSON_CAMERA = "camera";
	static constexpr const char* JSON_CAMERA_MATRIX = "matrix";
	static constexpr const char* JSON_CAMERA_POSITION = "position";
	
	void truck(float f);
	void dolly(float f);
	void boom(float f);

	void pan(const float degs);
	void tilt(const float degs);
	void roll(const float degs);

	const Matrix& rotation() const { return _rotation; }
	const Vector& position() const { return _position; }
	float planeDistance() const { return _planeDistance; }
	void setPosition(const Vector& position);
	void setMatrix(const Matrix& rotMatrix);

private:
	float convertToRads(const float degs);

private:
	Vector _position;
	Matrix _rotation;

	// Distance to the plane from camera
	float _planeDistance = 1.f;
};