#pragma once

#include "Utils.h" 
#include <map>

class PPMFile;

class Drawable
{
public:
	Drawable(float width, float height);

	virtual Color pixelColor(float posX, float posY) = 0;
	virtual void draw(PPMFile& file);

protected:
	const float _imageWidth = 0;
	const float _imageHeight = 0;
};

class Circle : public Drawable
{
public:
	Circle(float radius);
	void setPostion(float x, float y);
	Color pixelColor(float x, float y) override;

private:
	Color _color;
	float _radius = 0.f;
	float _cx = 0.f;
	float _cy = 0.f;
};

class Rectangles : public Drawable
{
public:
	Rectangles(int vCount, int hCount);
	void setColors(std::map<unsigned int, Color>&& colors);
	Color pixelColor(float x, float y) override;

private:
	float _boxWidth = 0;
	float _boxHeight = 0;
	std::map<unsigned int, Color> _rectColors;
	bool _useRandomColors = true;
	
};