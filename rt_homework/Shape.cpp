#include "Shape.h"
#include "PPMFile.h"
#include "Common.h"

using namespace std;

Drawable::Drawable(int width, int height) :
	_imageWidth(width), _imageHeight(height) 
{
}

void Drawable::draw(const std::string& filename)
{
	PPMFile file(filename, _imageWidth, _imageHeight, MAX_COLOR_COMPONENT);
	for (int y = 0; y < _imageHeight; ++y) {
		for (int x = 0; x < _imageWidth; ++x) {
			file.stream() << pixelColor((float)x, (float)y).toString();
		}
		file.stream() << endl;
	}
}

Circle::Circle(float radius) :
	Drawable(IMAGE_WIDTH, IMAGE_HEIGHT),
	_radius(radius* radius)
{
	_color = ColorRGB::random(MAX_COLOR_COMPONENT);
}

void Circle::setPostion(float x, float y) {
	_cx = x;
	_cy = y;
}

ColorRGB Circle::pixelColor(float x, float y) {
	float distance = (_cx - x) * (_cx - x) + (_cy - y) * (_cy - y);
	if (distance <= _radius) {
		return _color;
	}
	else {
		return ColorRGB(30, 30, 30);
	}
}

Rectangles::Rectangles(int vCount, int hCount) :
	Drawable(IMAGE_WIDTH, IMAGE_HEIGHT)
{
	_boxWidth = hCount > 0 ? (float)_imageWidth / hCount : (float)_imageWidth;
	_boxHeight = vCount > 0 ? (float)_imageHeight / vCount : (float)_imageHeight;
}

void Rectangles::setColors(std::map<unsigned int, ColorRGB>&& colors)
{
	if(colors.empty()) {
		return;
	}
	_useRandomColors = false;
	_rectColors.swap(colors);
}

ColorRGB Rectangles::pixelColor(float x, float y) {
	short boxX = (short)(x / _boxWidth);
	short boxY = (short)(y / _boxHeight);
	unsigned int idx = (boxY << 16) | boxX;

	if (_useRandomColors && ((int)x % 2 || (int)y % 2)) {
		return ColorRGB::random(MAX_COLOR_COMPONENT);
	}
	else {
		auto it = _rectColors.find(idx);
		if (it != _rectColors.end()) {
			return it->second;
		}
		else {
			auto it = _rectColors.emplace(idx, ColorRGB::random(MAX_COLOR_COMPONENT));
			return it.first->second;
		}
	}
};