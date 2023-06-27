#pragma once

#include "FrameBuffer.h"
#include "Renderer.h"

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
    void close();

private:
    bool loadTextureFromBuffer(int width, int height, GLuint* texture);
    void renderAsync();
    void keyboardEvents();

private:
    std::stringstream _output;
    FrameBuffer _buffer;
    bool _bufferUpdate = false;
    GLuint _imageTexture = 0;
    int _elapsedTime = 0;
    std::future<int> _future;
    bool _rendering = true;
    char _sceneFilePath[_MAX_PATH] = {"input\\task13\\scene0.crtscene"};
    const float _stepMove = 0.4f;
    const float _stepRot = 0.2f;
    std::unique_ptr<Renderer> _renderer;
    std::shared_ptr<Scene> _scene;
};

