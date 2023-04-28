#pragma once

#include "Utils.h" 
#include <map>

class Drawable
{
public:
	Drawable(int width, int height);

	virtual ColorRGB pixelColor(float posX, float posY) = 0;
	virtual void draw(const std::string& filename);

protected:
	const int _imageWidth = 0;
	const int _imageHeight = 0;
};

class Circle : public Drawable
{
public:
	Circle(float radius);
	void setPostion(float x, float y);
	ColorRGB pixelColor(float x, float y) override;

private:
	ColorRGB _color;
	float _radius = 0.f;
	float _cx = 0.f;
	float _cy = 0.f;
};

class Rectangles : public Drawable
{
public:
	Rectangles(int vCount, int hCount);
	void setColors(std::map<unsigned int, ColorRGB>&& colors);
	ColorRGB pixelColor(float x, float y) override;

private:
	float _boxWidth = 0;
	float _boxHeight = 0;
	std::map<unsigned int, ColorRGB> _rectColors;
	bool _useRandomColors = true;
	
};