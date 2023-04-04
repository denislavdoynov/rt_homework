#include "Shape.h"
#include "PPMFile.h"
#include "Common.h"

using namespace std;

Drawable::Drawable(float width, float height) :
	_imageWidth(width), _imageHeight(height) 
{
}

void Drawable::draw(PPMFile& file)
{
	for (float y = 0; y < _imageHeight; ++y) {
		for (float x = 0; x < _imageWidth; ++x) {
			file.stream() << pixelColor(x, y).toString();
		}
		file.stream() << endl;
	}
}

Circle::Circle(float radius) :
	Drawable(IMAGE_WIDTH, IMAGE_HEIGHT),
	_radius(radius* radius)
{
	_color = Color::random(MAX_COLOR_COMPONENT);
}

void Circle::setPostion(float x, float y) {
	_cx = x;
	_cy = y;
}

Color Circle::pixelColor(float x, float y) {
	float distance = (_cx - x) * (_cx - x) + (_cy - y) * (_cy - y);
	if (distance <= _radius) {
		return _color;
	}
	else {
		return Color(30, 30, 30);
	}
}

Rectangles::Rectangles(int vCount, int hCount) :
	Drawable(IMAGE_WIDTH, IMAGE_HEIGHT)
{
	_boxWidth = hCount > 0 ? _imageWidth / hCount : _imageWidth;
	_boxHeight = vCount > 0 ? _imageHeight / vCount : _imageHeight;
}

void Rectangles::setColors(std::map<unsigned int, Color>&& colors)
{
	if(colors.empty()) {
		return;
	}
	_useRandomColors = false;
	_rectColors.swap(colors);
}

Color Rectangles::pixelColor(float x, float y) {
	short boxX = (short)(x / _boxWidth);
	short boxY = (short)(y / _boxHeight);
	unsigned int idx = (boxY << 16) | boxX;

	if (_useRandomColors && ((int)x % 2 || (int)y % 2)) {
		return Color::random(MAX_COLOR_COMPONENT);
	}
	else {
		auto it = _rectColors.find(idx);
		if (it != _rectColors.end()) {
			return it->second;
		}
		else {
			auto it = _rectColors.emplace(idx, Color::random(MAX_COLOR_COMPONENT));
			return it.first->second;
		}
	}
};