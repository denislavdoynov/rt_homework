#pragma once

#include "Shape.h"

class RTImage : public Shape
{
public:
	RTImage();
	Color pixelColor(float x, float y) override;
};