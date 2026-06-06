#pragma once
#include <string>
#include <vector>
#include <GLES3/gl3.h>

struct CreditEntry {
    std::string role;
    std::string names;
    float r, g, b;
};

class CreditsScreen {
public:
    CreditsScreen();
    void init();
    void update(float dt);
    void render();
    bool isFinished() const { return mFinished; }
    void shutdown();
private:
    std::vector<CreditEntry> mEntries;
    float mScrollY;
    bool mFinished;
    float mFadeAlpha;
    float mTimer;
    GLuint mBackgroundTexture;
    GLuint mVAO, mVBO;
    void createBackgroundTexture();
    void drawBackground();
};
