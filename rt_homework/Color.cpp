#include "Color.h"
#include <sstream>

using namespace std;
Color::Color(int r, int g, int b) :
	R(r), G(g), B(b) {
}

/*static*/ Color Color::random(int maxColorComponent)
{
	return Color(rand() % maxColorComponent, rand() % maxColorComponent, rand() % maxColorComponent);
}

string Color::toString() {
	stringstream ss;
	ss << R << " " << G << " " << B << "\t";
	return ss.str();
}