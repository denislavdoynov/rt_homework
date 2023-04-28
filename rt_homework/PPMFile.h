#pragma once

#include <fstream>
#include <vector>
#include "Utils.h"

class FrameBuffer;

class PPMFile
{
public:
	PPMFile(const std::string& filename, int imageWidth, int imageHeight, int maxColorComponent);
	~PPMFile();
	void writeFrameBuffer(const FrameBuffer& framebuffer);
	operator std::ofstream& () { return _ppmFileStream; }
	std::ofstream& stream() { return _ppmFileStream; }

private:
	std::ofstream _ppmFileStream;
	int _maxColorComponent = 255;
};