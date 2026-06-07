#include "menu.h"
#include "font.h"
#include "input.h"
#include "utils.h"
#include <GLES3/gl3.h>
#include <string>

static const char* kOptions[] = { "JOUER", "CREDITS", "QUITTER" };
static const int kOptionCount = 3;

MenuScreen::MenuScreen()
    : mSelected(0), mAnimTime(0), mInputCooldown(0),
      mStartGame(false), mShowCredits(false), mQuit(false),
      mBgVAO(0), mBgVBO(0), mBgProg(0) {}

void MenuScreen::init() {
    LOGI("MenuScreen::init");
    mSelected = 0;
    mAnimTime = 0;
    mInputCooldown = 0.2f;
    mStartGame = mShowCredits = mQuit = false;
}

void MenuScreen::clearActions() {
    mStartGame = mShowCredits = mQuit = false;
    mInputCooldown = 0.2f;
}

void MenuScreen::ensureBackground() {
    // Pas de fond custom : on s'appuie sur glClear.
}

void MenuScreen::update(float dt) {
    mAnimTime += dt;
    if (mInputCooldown > 0) { mInputCooldown -= dt; return; }

    Input& in = Input::get();
    if (in.isMenuUp())   { mSelected = (mSelected + kOptionCount - 1) % kOptionCount; mInputCooldown = 0.2f; }
    if (in.isMenuDown()) { mSelected = (mSelected + 1) % kOptionCount;                mInputCooldown = 0.2f; }
    if (in.isMenuConfirm()) {
        if (mSelected == 0) mStartGame = true;
        else if (mSelected == 1) mShowCredits = true;
        else if (mSelected == 2) mQuit = true;
        mInputCooldown = 0.4f;
    }
}

void MenuScreen::render() {
    // Fond bleu nuit clairement visible — confirme que le menu rend bien.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.08f, 0.10f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Font& f = Font::get();
    // Titre
    f.drawText("RELIC SEEKER", -0.62f, 0.55f, 0.10f, 1.0f, 0.85f, 0.30f, 1.0f);
    f.drawText("Echoes of Eternity", -0.45f, 0.38f, 0.05f, 0.9f, 0.9f, 1.0f, 1.0f);

    // Options
    float y = 0.05f;
    for (int i = 0; i < kOptionCount; i++) {
        bool sel = (i == mSelected);
        float r = sel ? 1.0f : 0.7f;
        float g = sel ? 0.95f : 0.7f;
        float b = sel ? 0.4f  : 0.7f;
        float scale = sel ? 0.075f : 0.06f;
        const char* prefix = sel ? "> " : "  ";
        std::string line = std::string(prefix) + kOptions[i];
        f.drawText(line, -0.22f, y - i * 0.16f, scale, r, g, b, 1.0f);
    }

    f.drawText("Haut/Bas : naviguer  -  Tap : valider",
               -0.55f, -0.85f, 0.035f, 0.6f, 0.7f, 0.85f, 1.0f);

    glEnable(GL_DEPTH_TEST);
}

int  MenuScreen::getSelection() const   { return mSelected; }
bool MenuScreen::shouldStartGame()  const { return mStartGame; }
bool MenuScreen::shouldShowCredits() const { return mShowCredits; }
bool MenuScreen::shouldQuit()       const { return mQuit; }
