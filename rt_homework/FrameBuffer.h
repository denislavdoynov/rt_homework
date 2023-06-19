#pragma once

#include "Vector.h"
#include <vector>
#include <mutex>

class FrameBuffer
{
public:
    FrameBuffer() = default;
    FrameBuffer(size_t size);
    ~FrameBuffer();

    FrameBuffer& operator=(const FrameBuffer& rhs);
    Color& operator[](int index);    

    void genImageData();
    size_t size() const { return _buffer.size(); }
    const std::vector<Color>& buffer() const { return _buffer; }

    unsigned char* lockImageData();
    void unlockImageData();


private:
    std::vector<Color> _buffer;
    unsigned char* _imageData = nullptr;
    size_t _size = 0;
};

