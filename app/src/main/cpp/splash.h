#pragma once
#ifdef DESKTOP_BUILD
#include <GL/glew.h>
#else
#include <GLES3/gl3.h>
#endif

class SplashScreen {
public:
    SplashScreen();
    ~SplashScreen();
    bool init();
    void update(float dt);
    void render();
    bool isFinished() const { return mFinished; }
    void shutdown();
private:
    bool mFinished;
    float mTimer;
    GLuint mTexture;
    GLuint mVBO, mVAO;
    void createQuad();
};
