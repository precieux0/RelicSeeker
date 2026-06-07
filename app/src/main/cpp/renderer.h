#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "utils.h"

class Renderer {
public:
    Renderer();
    bool init(ANativeWindow* window);
    void onResize(ANativeWindow* window);
    void shutdown();
    void beginFrame();
    void endFrame();
    void setCamera(const Camera& cam);
    void setEnvironment(const vec3& skyColor, const vec3& fogColor, float fogDensity, const vec3& lightDir);
    void drawMesh(Mesh* mesh, const mat4& model, const vec3& color, float emissive = 0.0f);
    void drawSky();
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
private:
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    Shader mShader;
    Camera mCurrentCam;
    mat4 mView, mProj;
    vec3 mSkyColor, mFogColor, mLightDir;
    float mFogDensity;
    int mWidth, mHeight;
    bool mInitialized;
};
