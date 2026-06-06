#pragma once
#include <android_native_app_glue.h>

class Input {
public:
    static Input& get();
    void init(android_app* app);
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
    android_app* mApp;
    bool mLeft, mRight, mForward, mBack, mJump, mAction;
    bool mPause, mMenuUp, mMenuDown, mMenuConfirm;
    bool mUiMenuMode;
    float mTouchX, mTouchY;
    static int32_t handleInput(android_app* app, AInputEvent* event);
    void clearMovement();
};
