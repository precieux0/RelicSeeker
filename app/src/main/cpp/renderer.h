#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "shader.h"
#include "mesh.h"
#include "camera.h"

class Renderer {
public:
    void init(ANativeWindow* window);
    void shutdown();
    void beginFrame();
    void endFrame();
    void setCamera(const Camera& cam);
    void drawMesh(Mesh* mesh, const mat4& model, const vec3& color);
private:
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    Shader mShader;
    Camera mCurrentCam;
    mat4 mView, mProj;
    int mWidth, mHeight;
};
