#include "renderer.h"
#include "utils.h"
#include <android/native_window.h>

void Renderer::init(ANativeWindow* window) {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(mDisplay, nullptr, nullptr);
    const EGLint attribs[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);
    mSurface = eglCreateWindowSurface(mDisplay, config, window, nullptr);
    const EGLint ctxAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, ctxAttribs);
    eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
    glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    mShader.load(nullptr, nullptr);
    mShader.use();
    LOGI("Renderer initialized");
}

void Renderer::shutdown() {
    eglDestroyContext(mDisplay, mContext);
    eglDestroySurface(mDisplay, mSurface);
    eglTerminate(mDisplay);
}

void Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Récupérer les matrices depuis la caméra
    mView = Camera::getViewMatrix(); // à implémenter
    // Pour l'exemple, on suppose que la caméra expose view/proj
}

void Renderer::endFrame() { eglSwapBuffers(mDisplay, mSurface); }

void Renderer::drawMesh(Mesh* mesh, const mat4& model, const vec3& color) {
    mat4 mvp = mProj * mView * model;
    glUniformMatrix4fv(mShader.getUniformLocation("uMVP"), 1, GL_FALSE, mvp.m);
    glUniform3f(mShader.getUniformLocation("uColor"), color.x, color.y, color.z);
    vec3 lightDir(1,1,0); lightDir = lightDir.normalized();
    glUniform3f(mShader.getUniformLocation("uLightDir"), lightDir.x, lightDir.y, lightDir.z);
    mesh->draw(); // on implémente draw dans Mesh utilisant le VAO
}
