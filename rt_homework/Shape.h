#pragma once

#include "Color.h" 
#include <map>

class PPMFile;

class Shape
{
public:
	Shape(int width, int height);

	virtual Color pixelColor(int posX, int posY) = 0;
	virtual void draw(PPMFile& file);

protected:
	const int _imageWidth = 0;
	const int _imageHeight = 0;
};

class Circle : public Shape
{
public:
	Circle(float radius);
	void setPostion(float x, float y);
	Color pixelColor(int x, int y) override;

private:
	Color _color;
	float _radius = 0.f;
	float _cx = 0.f;
	float _cy = 0.f;
};

class Rectangles : public Shape
{
public:
	Rectangles(int vCount, int hCount);
	void setColors(std::map<unsigned int, Color>&& colors);
	Color pixelColor(int x, int y) override;

private:
	int _boxWidth = 0;
	int _boxHeight = 0;
	std::map<unsigned int, Color> _rectColors;
	bool _useRandomColors = true;
	
};