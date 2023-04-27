#include "PPMFile.h"

using namespace std;

PPMFile::PPMFile(const string& filename, int imageWidth, int imageHeight, int maxColorComponent) :
	_ppmFileStream(filename, std::ios::out | std::ios::binary) {
	_ppmFileStream << "P3" << endl;
	_ppmFileStream << imageWidth << " " << imageHeight << endl;
	_ppmFileStream << maxColorComponent << endl;
	_maxColorComponent = maxColorComponent;
}

void PPMFile::writeFrameBuffer(const vector<Vector>& framebuffer) 
{
	for (const auto& item : framebuffer) {
		_ppmFileStream << item.toColor(_maxColorComponent).toString();
	}
	_ppmFileStream << std::endl;
}


PPMFile::~PPMFile() {
	_ppmFileStream.close();
}