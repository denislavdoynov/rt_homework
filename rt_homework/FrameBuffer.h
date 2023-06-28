#pragma once

#include "Vector.h"
#include <vector>
#include <mutex>

class FrameBuffer
{
public:
    FrameBuffer() = default;
    FrameBuffer(uint32_t width, uint32_t height);
    ~FrameBuffer();

    FrameBuffer& operator=(const FrameBuffer& rhs);
    Color& operator[](int index);    

    void genImageData();
    size_t size() const { return _buffer.size(); }
    uint32_t width() const { return _width; }
    uint32_t height() const { return _height; }
    const std::vector<Color>& buffer() const { return _buffer; }

    unsigned char* lockImageData();
    void unlockImageData();


private:
    std::vector<Color> _buffer;
    unsigned char* _imageData = nullptr;
    uint32_t _width = 0;
    uint32_t _height = 0;
};

