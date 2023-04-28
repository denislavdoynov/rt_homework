#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height)
{
    _buffer.reserve(width * height);
}

void FrameBuffer::push(Color&& color) 
{
    _buffer.emplace_back(color);
}




