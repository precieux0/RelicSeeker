#include "menu.h"
#include "utils.h"
#include <GLES3/gl3.h>

MenuScreen::MenuScreen() : mSelected(0), mAnimTime(0), mInputCooldown(0),
      mStartGame(false), mShowCredits(false), mQuit(false),
      mBgVAO(0), mBgVBO(0), mBgProg(0) {}

void MenuScreen::init() {
    LOGI("MenuScreen::init called");
}

void MenuScreen::update(float dt) {}

void MenuScreen::render() {
    LOGI("MenuScreen::render called");
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

int MenuScreen::getSelection() const { return 0; }
bool MenuScreen::shouldStartGame() const { return false; }
bool MenuScreen::shouldShowCredits() const { return false; }
bool MenuScreen::shouldQuit() const { return false; }
void MenuScreen::clearActions() {}
