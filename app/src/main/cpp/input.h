#pragma once
#ifndef DESKTOP_BUILD
#include <android_native_app_glue.h>
#else
#include <GLFW/glfw3.h>
#endif

class Input {
public:
    static Input& get();
#ifdef DESKTOP_BUILD
    void init(GLFWwindow* window);
#else
    void init(android_app* app);
#endif
    void update();
    void setUiMode(bool menuMode);
    bool isLeft() const;
    bool isRight() const;
    bool isForward() const;
    bool isBack() const;
    bool isJump() const;
    bool isAction() const;
    bool isPause() const;
    bool isMenuUp() const;
    bool isMenuDown() const;
    bool isMenuConfirm() const;
    float getLookX() const;
    float getLookY() const;
private:
    Input();
#ifdef DESKTOP_BUILD
    GLFWwindow* mWindow;
#else
    android_app* mApp;
#endif
    bool mLeft, mRight, mForward, mBack, mJump, mAction;
    bool mPause, mMenuUp, mMenuDown, mMenuConfirm;
    bool mUiMenuMode;
    float mTouchX, mTouchY;
#ifndef DESKTOP_BUILD
    static int32_t handleInput(android_app* app, AInputEvent* event);
#endif
    void clearMovement();
};
