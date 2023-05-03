#pragma once

#include <vector>
#include "Utils.h"

class FrameBuffer
{
public:
    FrameBuffer() = default;
    FrameBuffer(size_t size);
    ~FrameBuffer();

    FrameBuffer& operator=(const FrameBuffer& rhs);
    void push(Color&& color);
    Color& operator[](int index);    

    size_t size() const { return _buffer.size(); }
    const std::vector<Color>& buffer() const { return _buffer; }
    unsigned char* imageData();


private:
    std::vector<Color> _buffer;
    unsigned char* _imageData = nullptr;
    size_t _size = 0;
};

