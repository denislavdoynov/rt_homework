#pragma once

#include "FrameBuffer.h"

#include <sstream>
#include <future>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

struct ImFont;
class Application
{
public:
    void renderTasks();
    void renderUI(ImFont* font);

private:
    bool loadTextureFromBuffer(int width, int height, GLuint* texture);

private:
    std::stringstream _output;
    FrameBuffer _buffer;
    GLuint _imageTexture = 0;
    int _elapsedTime = 0;
    std::future<int> _future;
    bool _rendering = true;
};

