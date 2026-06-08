#ifdef DESKTOP_BUILD
#include <GL/glew.h>
#else
#include <GLES3/gl3.h>
#endif

#pragma once

class MenuScreen {
public:
    MenuScreen();
    void init();
    void update(float dt);
    void render();
    int getSelection() const;
    bool shouldStartGame() const;
    bool shouldShowCredits() const;
    bool shouldQuit() const;
    void clearActions();
private:
    int mSelected;
    float mAnimTime;
    float mInputCooldown;
    bool mStartGame;
    bool mShowCredits;
    bool mQuit;
    GLuint mBgVAO, mBgVBO;
    GLuint mBgProg;
    void ensureBackground();
};
