#include "FrameBuffer.h"
#include <assert.h>

static std::mutex m;

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
    
    return *this;
}

void FrameBuffer::genImageData()
{
    const std::lock_guard<std::mutex> lock(m);
    if (_imageData) {
        delete[] _imageData;
        _imageData = nullptr;
    }

    _imageData = new unsigned char[_buffer.size() * 3];
    int i = 0;
    for (const auto& b : _buffer) {
        auto pixel = b.toColor(255);
        _imageData[i++] = pixel.R;
        _imageData[i++] = pixel.G;
        _imageData[i++] = pixel.B;
    }
}

unsigned char* FrameBuffer::lockImageData() 
{
    m.lock();
    return _imageData;
}

void FrameBuffer::unlockImageData()
{
    m.unlock();
}

Color& FrameBuffer::operator[](int index) {
    assert(index < _buffer.size());
    return _buffer[index];
}