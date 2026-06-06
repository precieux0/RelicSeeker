#include "input.h"
#include "utils.h"
#include <android/native_window.h>

Input& Input::get() {
    static Input sInstance;
    return sInstance;
}

Input::Input() : mApp(nullptr), mLeft(false), mRight(false), mForward(false),
                 mBack(false), mJump(false), mAction(false), mTouchX(0), mTouchY(0) {}

void Input::init(android_app* app) {
    mApp = app;
    app->onInputEvent = handleInput;
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
    Input& self = Input::get();
    if (!app || !app->window) return 0; // sécurité : fenêtre peut être nulle
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int width  = ANativeWindow_getWidth(app->window);
        int height = ANativeWindow_getHeight(app->window);
        if (width <= 0 || height <= 0) return 0;
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        if (x < width/2.0f) {
            self.mForward = (y < height * 0.35f);
            self.mBack    = (y > height * 0.65f);
            self.mLeft    = (x < width/4.0f);
            self.mRight   = (x > width/4.0f && x < width/2.0f);
        } else {
            self.mJump   = (y < height * 0.5f);
            self.mAction = (y >= height * 0.5f);
            self.mTouchX = (x - width/2.0f) / (width/2.0f);
            self.mTouchY = y / (float)height;
        }
        return 1;
    }
    return 0;
}
