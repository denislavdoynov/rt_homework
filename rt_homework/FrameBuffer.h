#pragma once

#include <vector>
#include "Utils.h"

class FrameBuffer
{
public:
    FrameBuffer(size_t size);
    void push(Color&& color);
    Color& operator[](int index);    

    size_t size() const {
        return _buffer.size(); 
    }

    const std::vector<Color>& buffer() const {
        return _buffer;
    }

private:
   std::vector<Color> _buffer;
};

