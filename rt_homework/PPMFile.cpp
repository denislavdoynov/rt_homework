#include "PPMFile.h"

using namespace std;

PPMFile::PPMFile(const string& filename, int imageWidth, int imageHeight, int maxColorComponent) :
	_ppmFileStream(filename, std::ios::out | std::ios::binary) {
	_ppmFileStream << "P3" << endl;
	_ppmFileStream << imageWidth << " " << imageHeight << endl;
	_ppmFileStream << maxColorComponent << endl;
}

PPMFile::~PPMFile() {
	_ppmFileStream.close();
}