#include "FrameBuffer.h"
#include <assert.h>

FrameBuffer::FrameBuffer(size_t size)
{
    _buffer.resize(size);
}

void FrameBuffer::push(Color&& color) 
{
    _buffer.emplace_back(std::forward<Color>(color));
}

Color& FrameBuffer::operator[](int index) {
	if (index >= _buffer.size()) {
		assert(false);
	}

	return _buffer[index];
}


