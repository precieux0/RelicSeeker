#include "renderer_desktop.h"
#include <iostream>

Renderer::Renderer()
    : mWindow(nullptr), mSkyColor(0.12f, 0.14f, 0.22f), mFogColor(0.10f, 0.12f, 0.18f),
      mLightDir(0.3f, 1, 0.2f), mFogDensity(0.02f), mWidth(0), mHeight(0) {}

bool Renderer::init(GLFWwindow* window) {
    if (!window) return false;
    mWindow = window;
    glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
    if (mWidth <= 0 || mHeight <= 0) return false;
    glViewport(0, 0, mWidth, mHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void Renderer::shutdown() {
    mWindow = nullptr;
}

void Renderer::onResize(GLFWwindow* window) {
    if (!window) return;
    mWindow = window;
    glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
    if (mWidth > 0 && mHeight > 0) glViewport(0, 0, mWidth, mHeight);
}

void Renderer::beginFrame() {
    if (!mWindow) return;
    if (mWidth > 0 && mHeight > 0) glViewport(0, 0, mWidth, mHeight);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glClearColor(mSkyColor.x, mSkyColor.y, mSkyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    if (!mWindow) return;
    glfwSwapBuffers(mWindow);
}

void Renderer::setCamera(const Camera& cam) { mCurrentCam = cam; }

void Renderer::setEnvironment(const vec3& skyColor, const vec3& fogColor, float fogDensity, const vec3& lightDir) {
    mSkyColor = skyColor;
    mFogColor = fogColor;
    mFogDensity = fogDensity;
    mLightDir = lightDir.normalized();
}
