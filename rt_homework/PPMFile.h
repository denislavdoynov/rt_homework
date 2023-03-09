#pragma once

#include <fstream>

class PPMFile
{
public:
	PPMFile(const std::string& filename, int imageWidth, int imageHeight, int maxColorComponent);
	~PPMFile();
	operator std::ofstream& () { return _ppmFileStream; }
	std::ofstream& stream() { return _ppmFileStream; }

private:
	std::ofstream _ppmFileStream;
};