#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer_desktop.h"
#include "input.h"
#include "font.h"
#include "splash.h"
#include "menu.h"
#include <chrono>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Relic Seeker Desktop", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Renderer renderer;
    if (!renderer.init(window)) {
        std::cerr << "Renderer init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Font& font = Font::get();
    if (!font.init()) {
        std::cerr << "Font init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Input& input = Input::get();
    input.init(window);

    SplashScreen splash;
    MenuScreen menu;
    if (!splash.init()) {
        std::cerr << "Splash init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    menu.init();

    auto lastTime = std::chrono::steady_clock::now();
    bool showMenu = false;
    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        if (!showMenu) {
            splash.update(dt);
            if (splash.isFinished()) showMenu = true;
        }

        glfwPollEvents();
        input.setUiMode(showMenu);
        input.update();
        if (showMenu) {
            menu.update(dt);
            if (menu.shouldQuit()) break;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0) renderer.onResize(window);

        renderer.beginFrame();
        if (!showMenu) {
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            splash.render();
        } else {
            menu.render();
            font.drawText("PC BUILD", -0.25f, -0.8f, 0.05f, 0.8f, 0.9f, 1.0f, 1.0f);
        }
        renderer.endFrame();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
    }

    font.shutdown();
    splash.shutdown();
    renderer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
