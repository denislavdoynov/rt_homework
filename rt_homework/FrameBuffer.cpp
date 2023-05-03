#include "FrameBuffer.h"
#include <assert.h>

FrameBuffer::FrameBuffer(size_t size)
{
    _buffer.resize(size);
}

FrameBuffer::~FrameBuffer()
{
    if (_imageData) {
        delete[] _imageData;
        _imageData = nullptr;
    }
}

FrameBuffer& FrameBuffer::operator=(const FrameBuffer& rhs)
{
    _buffer = rhs._buffer;
    _size = rhs._size;
    if(_imageData) {
        delete[] _imageData;
        _imageData = nullptr;
    }
    return *this;
}

void FrameBuffer::push(Color&& color) 
{
    _buffer.emplace_back(std::forward<Color>(color));
}

unsigned char* FrameBuffer::imageData() 
{
    if(_imageData)
        return _imageData;

    _imageData = new unsigned char[_buffer.size() * 3];
    int i = 0;
    for (const auto& b : _buffer) {
        auto pixel = b.toColor(255);
        _imageData[i++] = pixel.R;
        _imageData[i++] = pixel.G;
        _imageData[i++] = pixel.B;
    }

    return _imageData;
}

Color& FrameBuffer::operator[](int index) {
	if (index >= _buffer.size()) {
		assert(false);
	}

	return _buffer[index];
}