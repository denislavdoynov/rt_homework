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