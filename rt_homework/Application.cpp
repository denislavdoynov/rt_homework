#include "Application.h"
#include "Shape.h"
#include "Scene.h"
#include "Common.h"

#include "imgui.h"

//#define TASK_13

int renderAllTasks(FrameBuffer& buffer, std::stringstream& log);

bool Application::loadTextureFromBuffer(GLuint* texture)
{
    if(_buffer.size() == 0)
        return false;

    // Create a OpenGL texture identifier
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _buffer.width(), _buffer.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, _buffer.lockImageData());

    _buffer.unlockImageData();
    *texture = imageTexture;

    return true;
}

void Application::renderTasks()  {
    _rendering = true;
    _future = std::async(std::launch::async, [this]() {
        return renderAllTasks(_buffer, _output);
    });
}
void Application::renderAsync() {
    if(!_rendering) {
        _rendering = true;
        _future = std::async(std::launch::async, [this]() {
            if(_renderer) 
                return _renderer->renderScene("", &_buffer, &_output);
            else 
                return 0;
        });
    }
}

void Application::keyboardEvents() 
{
    if (ImGui::IsKeyPressed(ImGuiKey_D))
    {
        if (_scene) {
            _scene->camera().pan(_stepMove);
            _scene->camera().truck(-_stepRot);
            renderAsync();
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_A))
    {
        if (_scene) {
            _scene->camera().pan(-_stepMove);
            _scene->camera().truck(_stepRot);
            renderAsync();
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_W))
    {
        if (_scene) {
            _scene->camera().boom(-_stepRot);
            _scene->camera().tilt(-_stepMove);
            
            renderAsync();
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_S))
    {
        if (_scene) {
            _scene->camera().boom(_stepRot);
            _scene->camera().tilt(_stepMove);
            
            renderAsync();
        }
    }
}

void Application::close() 
{
    if(_renderer) {
        _renderer->abort();
    }
}

void Application::renderUI(ImFont* font)
{
    // Dialog
    ImGui::PushFont(font);
    ImGui::Begin("Scene Renderer", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
   
    ImGui::InputText("##Scene",_sceneFilePath, IM_ARRAYSIZE(_sceneFilePath));
    ImGui::SameLine();
    if (ImGui::Button("Local Render")) {
        _scene = std::make_shared<Scene>();
        _renderer = std::make_unique<Renderer>(_scene);
        if (_scene->loadScene(_sceneFilePath)) {
            renderAsync();
        }
    }

    // Process wsad keyboard events to move the camera
    keyboardEvents();

    if (_future.valid() && _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        _rendering = false;
        _elapsedTime = _future.get();
        _bufferUpdate = true;
    }

    if (_rendering) {
        ImGui::SameLine();
        ImGui::Text("Rendering...");
    }
    else {
        ImGui::SameLine();
        ImGui::Text("Finished in %u ms(s)", _elapsedTime);
    }

    if(_bufferUpdate) {
        loadTextureFromBuffer(&_imageTexture);
        _bufferUpdate = false;
    }
    ImGui::Image((void*)(intptr_t)_imageTexture, ImVec2(960, 540));
    ImGui::InputTextMultiline("##source", (char*)_output.str().c_str(), _output.str().size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 6 + 3), ImGuiInputTextFlags_ReadOnly);

    ImGui::PopFont();

    ImGui::End();
}

int renderAllTasks(FrameBuffer& buffer, std::stringstream& log)
{
    srand((unsigned int)time(NULL));
    int elapsedTime = 0;

#ifdef TASK_1		
    Rectangles rect(7, 5);
    rect.draw("output\\task1.ppm");

    Rectangles rect2(3, 0);
    rect2.setColors({
        std::make_pair(0, Color(255, 255, 255)),
        std::make_pair(1 << 16, Color(0, 233, 0)),
        std::make_pair(2 << 16, Color(233, 0, 0)),
        });
    rect2.draw("output\\task1_2.ppm");
#endif

#ifdef TASK_2
    Circle circle(220.f);
    circle.setPostion(IMAGE_WIDTH * 0.5f, IMAGE_HEIGHT * 0.5f);
    circle.draw("output\\task2.ppm");
#endif

#ifdef TASK_3
    Scene scene;
    scene.draw("output\\task3.ppm");
#endif

#ifdef TASK_4
    Vector p2_a(3.5f, 0.f, 0.f);
    std::cout << "cross: " << p2_a.cross({ 1.75f, 3.5f, 0.f }) << std::endl;

    Vector p2_b(3.f, -3.f, 1.f);
    std::cout << "cross: " << p2_b.cross({ 4.f, 9.f, 3.f }) << std::endl;

    Vector p2_c(3.5f, 0.f, 0.f);
    std::cout << "cross: " << p2_c.cross({ 1.75f, 3.5f, 0.f }) << std::endl;

    Triangle p3_a({ -1.75, -1.75, -3 }, { 1.75, -1.75, -3 }, { 0, 1.75, -3 });
    std::cout << "normal: " << p3_a.normal().normalize() << std::endl;
    std::cout << "area: " << p3_a.area() << std::endl;

    Triangle p3_b({ 0, 0, -1 }, { 1, 0, 1 }, { -1, 0, 1 });
    std::cout << "normal: " << p3_b.normal().normalize() << std::endl;
    std::cout << "area: " << p3_b.area() << std::endl;

    Triangle p3_c({ 0.56f, 1.11f, 1.23f }, { 0.44f, -2.368f, -0.54f }, { -1.56f, 0.15f, -1.92f });
    std::cout << "normal: " << p3_c.normal().normalize() << std::endl;
    std::cout << "area: " << p3_c.area() << std::endl;
#endif

#ifdef TASK_5
    {
        // Create triangle and add to the scene
        Triangle* t1 = new Triangle({ -1.75, -1.75, -3 }, { 3.75, -1.75, -3 }, { 0, 1.75, -3 });
        t1.setColor({ 3, 252, 11 });
        Triangle* t2 = new Triangle({ 0.70, -1.75, -2 }, { 4.75, 1.75, -3 }, { 0, 1.75, -3 });
        t2.setColor({ 252, 3, 140 });

        Scene scene;
        scene.addGeometry(t1);
        scene.addGeometry(t2);
        scene.draw("output\\task5.ppm");
    }
    {
        Scene scene;
        scene.addGeometry(new Triangle( { -1.75, -1.75, -3 }, { 1.75, -1.75, -30 }, { 0, 1.75, -20 } ));
        scene.draw("output\\task5_a.ppm");
    }

#endif

#ifdef TASK_6
    {
        // Create triangle and add to the scene
        Scene scene(IMAGE_WIDTH, IMAGE_HEIGHT);
        Renderer renderer(scene);
        Triangle* t1 = new Triangle({ -1.75, -1.75, -3 }, { 3.75, -1.75, -3 }, { 0, 1.75, -3 });
        t1.setColor({ 3, 252, 11 });
        Triangle* t2 = new Triangle({ 0.70, -1.75, -2 }, { 4.75, 1.75, -3 }, { 0, 1.75, -3 });
        t2.setColor({ 252, 3, 140 });

        scene.addGeometry(t1);
        scene.addGeometry(t2);

        scene.camera().pan(30);
        renderer.renderScene("output\\task6_1.ppm");

        scene.camera().pan(-30);
        renderer.renderScene("output\\task6_2.ppm");

        scene.camera().truck({ -2, 0, 0 });
        renderer.renderScene("output\\task6_2_1.ppm");

        float panRotation = 0.f;
        float step = 2.f;
        for (int i = 0; i < 50; i++) {
            panRotation += step;
            if (abs(panRotation) > 20.f)
                step *= -1;

            std::stringstream output;
            output << "output\\task6_3_" << i << ".ppm";
            scene.camera().pan(panRotation);
            renderer.renderScene(output.str());
        }
    }

#endif

#ifdef TASK_7
    Scene scene;
    Renderer renderer(scene);
    if (scene.loadScene("input\\task7\\scene0.crtscene")) {
        renderer.renderScene("output\\task7_0.ppm");
    }
    if (scene.loadScene("input\\task7\\scene1.crtscene")) {
        renderer.renderScene("output\\task7_1.ppm");
    }

    if (scene.loadScene("input\\task7\\scene2.crtscene")) {
        renderer.renderScene("output\\task7_2.ppm");
    }

    if (scene.loadScene("input\\task7\\scene3.crtscene")) {
        renderer.renderScene("output\\task7_3.ppm");
    }

    if (scene.loadScene("input\\task7\\scene4.crtscene")) {
        renderer.renderScene("output\\task7_4.ppm");
    }
#endif

#ifdef TASK_8
    Scene scene;
    Renderer renderer(scene);
    if (scene.loadScene("input\\task8\\scene0.crtscene")) {
        renderer.renderScene("output\\task8\\task8_0.ppm");
    }
    if (scene.loadScene("input\\task8\\scene1.crtscene")) {
        renderer.renderScene("output\\task8\\task8_1.ppm");
    }
    if (scene.loadScene("input\\task8\\scene2.crtscene")) {
        renderer.renderScene("output\\task8\\task8_2.ppm");
    }
    if (scene.loadScene("input\\task8\\scene3.crtscene")) {
        renderer.renderScene("output\\task8\\task8_3.ppm");
    }
#endif


#ifdef TASK_9
    Scene scene;
    Renderer renderer(scene);
    if (scene.loadScene("input\\task9\\scene0.crtscene")) {
        renderer.renderScene("output\\task9\\task9_0.ppm");
    }
    
    if (scene.loadScene("input\\task9\\scene1.crtscene")) {
        renderer.renderScene("output\\task9\\task9_1.ppm");
    }
    if (scene.loadScene("input\\task9\\scene2.crtscene")) {
        renderer.renderScene("output\\task9\\task9_2.ppm");
    }
    if (scene.loadScene("input\\task9\\scene3.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task9\\task9_3.ppm", &buffer, &log);
    }
    
    if (scene.loadScene("input\\task9\\scene4.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task9\\task9_4.ppm", &buffer, &log);
    }
    if (scene.loadScene("input\\task9\\scene5.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task9\\task9_5.ppm", &buffer, &log);
    }

#endif

#ifdef TASK_11
    Scene scene;
    Renderer renderer(scene);

    if (scene.loadScene("input\\task11\\scene0.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task0.ppm", &buffer, &log);
    }   
    if (scene.loadScene("input\\task11\\scene1.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task1.ppm", &buffer, &log);
    }
    if (scene.loadScene("input\\task11\\scene2.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task2.ppm", &buffer, &log);
    }
     if (scene.loadScene("input\\task11\\scene3.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task3.ppm", &buffer, &log);
    }  
    if (scene.loadScene("input\\task11\\scene4.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task4.ppm", &buffer, &log);
    }
    if (scene.loadScene("input\\task11\\scene5.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task5.ppm", &buffer, &log);
    }
    if (scene.loadScene("input\\task11\\scene6.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task6.ppm", &buffer, &log);
    }  
    if (scene.loadScene("input\\task11\\scene7.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task7.ppm", &buffer, &log);
    }
    if (scene.loadScene("input\\task11\\scene8.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task11\\task8.ppm", &buffer, &log);
    }

#endif

#ifdef TASK_12
    Scene scene;
    Renderer renderer(scene);

    if (scene.loadScene("input\\task12\\scene0.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task12\\task0.ppm", &buffer, &log);
    }
 
#endif

#ifdef TASK_13
    Scene scene;
    Renderer renderer(scene);
    
    if (scene.loadScene("input\\task13\\scene0.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task13\\task0.ppm", &buffer, &log);
    }
   
    if (scene.loadScene("input\\task13\\scene1.crtscene")) {
        elapsedTime += renderer.renderScene("output\\task13\\task1.ppm", &buffer, &log);
    }
 
#endif

    return elapsedTime;
}