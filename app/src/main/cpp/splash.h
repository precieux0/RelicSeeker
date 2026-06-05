#pragma once
#include <GLES3/gl3.h>

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
