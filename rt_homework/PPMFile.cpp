#include "PPMFile.h"
#include "FrameBuffer.h"
using namespace std;

PPMFile::PPMFile(const string& filename, int imageWidth, int imageHeight, int maxColorComponent) :
	_ppmFileStream(filename, std::ios::out | std::ios::binary) {
	_ppmFileStream << "P3" << endl;
	_ppmFileStream << imageWidth << " " << imageHeight << endl;
	_ppmFileStream << maxColorComponent << endl;
	_maxColorComponent = maxColorComponent;
}

void PPMFile::writeFrameBuffer(const FrameBuffer& framebuffer)
{
	for (const auto& pixel : framebuffer.buffer()) {
		_ppmFileStream << pixel.toColor(_maxColorComponent).toString();
	}
	_ppmFileStream << std::endl;
}


PPMFile::~PPMFile() {
	_ppmFileStream.close();
}