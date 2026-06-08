#pragma once

#ifdef DESKTOP_BUILD
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif
#include "utils.h"

class Renderer {
public:
    Renderer();
    bool init(GLFWwindow* window);
    void onResize(GLFWwindow* window);
    void shutdown();
    void beginFrame();
    void endFrame();
    void setCamera(const Camera& cam);
    void setEnvironment(const vec3& skyColor, const vec3& fogColor, float fogDensity, const vec3& lightDir);
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
private:
    GLFWwindow* mWindow;
    Camera mCurrentCam;
    mat4 mView, mProj;
    vec3 mSkyColor, mFogColor, mLightDir;
    float mFogDensity;
    int mWidth, mHeight;
};
