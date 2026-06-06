#include "input.h"
#include "utils.h"
#include <android/native_window.h>

Input& Input::get() {
    static Input instance;
    return instance;
}

Input::Input() : mApp(nullptr), mLeft(false), mRight(false), mForward(false),
                 mBack(false), mJump(false), mAction(false), mTouchX(0), mTouchY(0) {}

void Input::init(android_app* app) {
    mApp = app;
    app->onInputEvent = [](android_app* app, AInputEvent* event) -> int32_t {
        return Input::get().handleInput(app, event);
    };
}

void Input::update() {}

bool Input::isLeft()    const { return mLeft; }
bool Input::isRight()   const { return mRight; }
bool Input::isForward() const { return mForward; }
bool Input::isBack()    const { return mBack; }
bool Input::isJump()    const { return mJump; }
bool Input::isAction()  const { return mAction; }
float Input::getLookX() const { return mTouchX; }
float Input::getLookY() const { return mTouchY; }

int32_t Input::handleInput(android_app* app, AInputEvent* event) {
    if (!app || !app->window) return 0;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int width = ANativeWindow_getWidth(app->window);
        int height = ANativeWindow_getHeight(app->window);
        if (width <= 0 || height <= 0) return 0;
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        if (x < width / 2.0f) {
            // Moitié gauche : déplacement
            mForward = (y < height * 0.35f);
            mBack    = (y > height * 0.65f);
            mLeft    = (x < width / 4.0f);
            mRight   = (x > width / 4.0f && x < width / 2.0f);
        } else {
            // Moitié droite : saut et action
            mJump    = (y < height * 0.5f);
            mAction  = (y >= height * 0.5f);
            mTouchX  = (x - width / 2.0f) / (width / 2.0f);
            mTouchY  = y / (float)height;
        }
        return 1;
    }
    return 0;
}
