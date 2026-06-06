#pragma once
#include <android_native_app_glue.h>

class Input {
public:
    static Input& get();
    void init(android_app* app);
    void update();
    bool isLeft() const;
    bool isRight() const;
    bool isForward() const;
    bool isBack() const;
    bool isJump() const;
    bool isAction() const;
    float getLookX() const;
    float getLookY() const;
private:
    Input();
    android_app* mApp;
    bool mLeft, mRight, mForward, mBack, mJump, mAction;
    float mTouchX, mTouchY;
    static int32_t handleInput(android_app* app, AInputEvent* event);
};
