#include "input.h"

static Input* sInstance = nullptr;

Input& Input::get() { if (!sInstance) sInstance = new Input(); return *sInstance; }
void Input::init(android_app* app) { sInstance = &get(); app->onInputEvent = handleInput; }
void Input::update() {}
bool Input::isLeft() const { return mLeft; }
bool Input::isRight() const { return mRight; }
bool Input::isForward() const { return mForward; }
bool Input::isBack() const { return mBack; }
bool Input::isJump() const { return mJump; }
bool Input::isAction() const { return mAction; }
float Input::getLookX() const { return mTouchX; }
float Input::getLookY() const { return mTouchY; }

int32_t Input::handleInput(android_app*, AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        int width = ANativeWindow_getWidth(sInstance->mApp->window);
        if (x < width/2) {
            // zone gauche : déplacement
            sInstance->mForward = (y < 300);
            sInstance->mBack = (y > 600);
            sInstance->mLeft = (x < width/4);
            sInstance->mRight = (x > width/4 && x < width/2);
        } else {
            // zone droite : saut/action et regard
            sInstance->mJump = (y < 400);
            sInstance->mAction = (y >= 400);
            sInstance->mTouchX = (x - width/2) / (width/2);
            sInstance->mTouchY = y / ANativeWindow_getHeight(sInstance->mApp->window);
        }
        return 1;
    }
    return 0;
}
