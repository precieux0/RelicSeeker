#include "input.h"
#include "utils.h"
#ifndef DESKTOP_BUILD
#include <android/native_window.h>
#else
#include <GLFW/glfw3.h>
#endif

Input& Input::get() {
    static Input instance;
    return instance;
}

Input::Input()
#ifdef DESKTOP_BUILD
    : mWindow(nullptr), mLeft(false), mRight(false), mForward(false),
      mBack(false), mJump(false), mAction(false),
      mPause(false), mMenuUp(false), mMenuDown(false), mMenuConfirm(false),
      mUiMenuMode(false), mTouchX(0), mTouchY(0) {}
#else
    : mApp(nullptr), mLeft(false), mRight(false), mForward(false),
      mBack(false), mJump(false), mAction(false),
      mPause(false), mMenuUp(false), mMenuDown(false), mMenuConfirm(false),
      mUiMenuMode(false), mTouchX(0), mTouchY(0) {}
#endif

#ifdef DESKTOP_BUILD
void Input::init(GLFWwindow* window) {
    mWindow = window;
}
#endif

#ifndef DESKTOP_BUILD
void Input::init(android_app* app) {
    mApp = app;
    app->onInputEvent = [](android_app* app, AInputEvent* event) -> int32_t {
        return Input::get().handleInput(app, event);
    };
}
#endif

void Input::setUiMode(bool menuMode) { mUiMenuMode = menuMode; }
void Input::update() {
#ifdef DESKTOP_BUILD
    if (mWindow) {
        mPause = glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        mMenuUp = glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS;
        mMenuDown = glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS;
        mMenuConfirm = glfwGetKey(mWindow, GLFW_KEY_ENTER) == GLFW_PRESS ||
                       glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS;
    } else {
        mPause = mMenuUp = mMenuDown = mMenuConfirm = false;
    }
#else
    mPause = mMenuUp = mMenuDown = mMenuConfirm = false;
#endif
}

void Input::clearMovement() {
    mLeft = mRight = mForward = mBack = mJump = mAction = false;
    mTouchX = mTouchY = 0;
}

bool Input::isLeft()    const { return mLeft; }
bool Input::isRight()   const { return mRight; }
bool Input::isForward() const { return mForward; }
bool Input::isBack()    const { return mBack; }
bool Input::isJump()    const { return mJump; }
bool Input::isAction()  const { return mAction; }
bool Input::isPause()   const { return mPause; }
bool Input::isMenuUp()    const { return mMenuUp; }
bool Input::isMenuDown()  const { return mMenuDown; }
bool Input::isMenuConfirm() const { return mMenuConfirm; }
float Input::getLookX() const { return mTouchX; }
float Input::getLookY() const { return mTouchY; }

#ifndef DESKTOP_BUILD
int32_t Input::handleInput(android_app* app, AInputEvent* event) {
    Input& self = Input::get();
    if (!self.mApp || !self.mApp->window) return 0;
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) return 0;

    int action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_CANCEL) {
        self.clearMovement();
        return 1;
    }
    if (action != AMOTION_EVENT_ACTION_DOWN && action != AMOTION_EVENT_ACTION_MOVE) return 0;

    int width = ANativeWindow_getWidth(self.mApp->window);
    int height = ANativeWindow_getHeight(self.mApp->window);
    if (width <= 0 || height <= 0) return 0;

    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);

    if (self.mUiMenuMode) {
        self.mMenuUp = self.mMenuDown = self.mMenuConfirm = false;
        if (x < width * 0.5f) {
            self.mMenuUp = (y < height * 0.45f);
            self.mMenuDown = (y >= height * 0.45f);
        } else {
            self.mMenuConfirm = true;
        }
        return 1;
    }

    // Pause: coin superieur gauche
    if (x < width * 0.12f && y < height * 0.12f) {
        self.mPause = true;
        return 1;
    }

    if (x < width / 2.0f) {
        self.mForward = (y < height * 0.35f);
        self.mBack    = (y > height * 0.65f);
        self.mLeft    = (x < width / 4.0f);
        self.mRight   = (x > width / 4.0f && x < width / 2.0f);
        self.mJump = false;
        self.mAction = false;
    } else {
        self.mJump    = (y < height * 0.5f);
        self.mAction  = (y >= height * 0.5f);
        self.mTouchX  = (x - width * 0.75f) / (width * 0.25f);
        self.mTouchY  = (y - height * 0.5f) / (height * 0.5f);
        self.mLeft = self.mRight = self.mForward = self.mBack = false;
    }
    return 1;
}
#endif
