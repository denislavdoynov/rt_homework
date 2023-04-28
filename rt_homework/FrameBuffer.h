#pragma once

#include <vector>
#include "Utils.h"


class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    void push(Color&& color);
    const std::vector<Color>& getBuffer() const { return _buffer; }

private:
   std::vector<Color> _buffer;
};

