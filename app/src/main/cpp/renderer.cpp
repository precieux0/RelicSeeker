#include "renderer.h"
#include "utils.h"
#include <android/native_window.h>

Renderer::Renderer()
    : mDisplay(EGL_NO_DISPLAY), mSurface(EGL_NO_SURFACE), mContext(EGL_NO_CONTEXT),
      mSkyColor(0.12f, 0.14f, 0.22f), mFogColor(0.10f, 0.12f, 0.18f), mLightDir(0.3f, 1, 0.2f),
      mFogDensity(0.02f), mWidth(0), mHeight(0), mInitialized(false) {}

bool Renderer::init(ANativeWindow* window) {
    if (mInitialized) return true;
    if (!window) { LOGE("Renderer::init window is null"); return false; }

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) { LOGE("eglGetDisplay failed: 0x%x", eglGetError()); return false; }
    if (!eglInitialize(mDisplay, nullptr, nullptr)) { LOGE("eglInitialize failed: 0x%x", eglGetError()); return false; }

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs = 0;
    if (!eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs) || numConfigs < 1) {
        LOGE("eglChooseConfig failed"); return false;
    }

    EGLint format = 0;
    eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    mSurface = eglCreateWindowSurface(mDisplay, config, window, nullptr);
    if (mSurface == EGL_NO_SURFACE) { LOGE("eglCreateWindowSurface failed"); return false; }

    const EGLint ctxAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, ctxAttribs);
    if (mContext == EGL_NO_CONTEXT) {
        eglDestroySurface(mDisplay, mSurface);
        return false;
    }

    if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
        eglDestroyContext(mDisplay, mContext);
        eglDestroySurface(mDisplay, mSurface);
        mContext = EGL_NO_CONTEXT;
        mSurface = EGL_NO_SURFACE;
        return false;
    }

    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);
    if (mWidth <= 0 || mHeight <= 0) {
        mWidth = ANativeWindow_getWidth(window);
        mHeight = ANativeWindow_getHeight(window);
    }
    glViewport(0, 0, mWidth, mHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!mShader.load(nullptr, nullptr)) { LOGE("Shader load failed"); return false; }
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

void Renderer::setEnvironment(const vec3& skyColor, const vec3& fogColor, float fogDensity, const vec3& lightDir) {
    mSkyColor = skyColor;
    mFogColor = fogColor;
    mFogDensity = fogDensity;
    mLightDir = lightDir.normalized();
}

void Renderer::onResize(ANativeWindow* window) {
    if (!mInitialized || !window) return;
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mHeight);
    if (mWidth <= 0 || mHeight <= 0) {
        mWidth = ANativeWindow_getWidth(window);
        mHeight = ANativeWindow_getHeight(window);
    }
    if (mWidth > 0 && mHeight > 0) glViewport(0, 0, mWidth, mHeight);
    LOGI("Renderer resized %dx%d", mWidth, mHeight);
}

void Renderer::beginFrame() {
    if (!mInitialized) return;
    if (eglGetCurrentContext() != mContext) {
        if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
            LOGE("eglMakeCurrent failed in beginFrame: 0x%x", eglGetError());
            mInitialized = false;
            return;
        } else {
            LOGI("eglMakeCurrent recovered context in beginFrame");
        }
    }
    // Toujours réimposer le viewport — sécurité contre tout reset d'état.
    if (mWidth > 0 && mHeight > 0) glViewport(0, 0, mWidth, mHeight);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glClearColor(mSkyColor.x, mSkyColor.y, mSkyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mView = mCurrentCam.getViewMatrix();
    mProj = mCurrentCam.getProjectionMatrix(mHeight > 0 ? (float)mWidth / (float)mHeight : 1.0f);
}

void Renderer::endFrame() {
    if (!mInitialized) return;
    if (!eglSwapBuffers(mDisplay, mSurface)) {
        LOGE("eglSwapBuffers failed: 0x%x", eglGetError());
        mInitialized = false;
    }
}

void Renderer::setCamera(const Camera& cam) { mCurrentCam = cam; }

void Renderer::drawSky() {
    // Le ciel est defini par glClearColor dans beginFrame.
}

void Renderer::drawMesh(Mesh* mesh, const mat4& model, const vec3& color, float emissive) {
    if (!mInitialized || !mesh) return;
    mShader.use();
    mat4 mvp = mProj * mView * model;
    vec3 camPos = mCurrentCam.getPosition();
    glUniformMatrix4fv(mShader.getUniformLocation("uMVP"), 1, GL_FALSE, mvp.m);
    glUniformMatrix4fv(mShader.getUniformLocation("uModel"), 1, GL_FALSE, model.m);
    glUniform3f(mShader.getUniformLocation("uColor"), color.x, color.y, color.z);
    glUniform3f(mShader.getUniformLocation("uLightDir"), mLightDir.x, mLightDir.y, mLightDir.z);
    glUniform3f(mShader.getUniformLocation("uCameraPos"), camPos.x, camPos.y, camPos.z);
    glUniform3f(mShader.getUniformLocation("uFogColor"), mFogColor.x, mFogColor.y, mFogColor.z);
    glUniform1f(mShader.getUniformLocation("uFogDensity"), mFogDensity);
    glUniform1f(mShader.getUniformLocation("uEmissive"), emissive);
    mesh->draw();
}
