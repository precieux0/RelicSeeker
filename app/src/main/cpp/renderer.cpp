#include "renderer.h"
#include "utils.h"
#include <android/native_window.h>

Renderer::Renderer()
    : mDisplay(EGL_NO_DISPLAY), mSurface(EGL_NO_SURFACE), mContext(EGL_NO_CONTEXT),
      mWidth(0), mHeight(0), mInitialized(false) {}

bool Renderer::init(ANativeWindow* window) {
    if (mInitialized) {
        LOGI("Renderer already initialized");
        return true;
    }
    if (!window) {
        LOGE("Renderer::init window is null");
        return false;
    }

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed: 0x%x", eglGetError());
        return false;
    }
    if (!eglInitialize(mDisplay, nullptr, nullptr)) {
        LOGE("eglInitialize failed: 0x%x", eglGetError());
        return false;
    }

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs = 0;
    if (!eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs) || numConfigs < 1) {
        LOGE("eglChooseConfig failed (%d configs), error: 0x%x", numConfigs, eglGetError());
        return false;
    }

    EGLint format = 0;
    eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    mSurface = eglCreateWindowSurface(mDisplay, config, window, nullptr);
    if (mSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed: 0x%x", eglGetError());
        return false;
    }

    const EGLint ctxAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, ctxAttribs);
    if (mContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed: 0x%x", eglGetError());
        eglDestroySurface(mDisplay, mSurface);
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
        eglDestroyContext(mDisplay, mContext);
        eglDestroySurface(mDisplay, mSurface);
        return false;
    }

    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);
    if (mWidth <= 0 || mHeight <= 0) {
        mWidth = ANativeWindow_getWidth(window);
        mHeight = ANativeWindow_getHeight(window);
    }
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!mShader.load(nullptr, nullptr)) {
        LOGE("Shader load failed");
        return false;
    }
    mShader.use();

    LOGI("Renderer initialized, size %dx%d", mWidth, mHeight);
    mInitialized = true;
    return true;
}

void Renderer::shutdown() {
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mContext != EGL_NO_CONTEXT) eglDestroyContext(mDisplay, mContext);
        if (mSurface != EGL_NO_SURFACE) eglDestroySurface(mDisplay, mSurface);
        eglTerminate(mDisplay);
    }
    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
    mInitialized = false;
}

void Renderer::beginFrame() {
    if (!mInitialized) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mView = mCurrentCam.getViewMatrix();
    mProj = mCurrentCam.getProjectionMatrix((float)mWidth / (float)mHeight);
}

void Renderer::endFrame() {
    if (!mInitialized) return;
    eglSwapBuffers(mDisplay, mSurface);
}

void Renderer::setCamera(const Camera& cam) { mCurrentCam = cam; }

void Renderer::drawMesh(Mesh* mesh, const mat4& model, const vec3& color) {
    if (!mInitialized || !mesh) return;
    mShader.use();
    mat4 mvp = mProj * mView * model;
    glUniformMatrix4fv(mShader.getUniformLocation("uMVP"), 1, GL_FALSE, mvp.m);
    glUniform3f(mShader.getUniformLocation("uColor"), color.x, color.y, color.z);
    vec3 lightDir(1,1,0);
    lightDir = lightDir.normalized();
    glUniform3f(mShader.getUniformLocation("uLightDir"), lightDir.x, lightDir.y, lightDir.z);
    mesh->draw();
}
