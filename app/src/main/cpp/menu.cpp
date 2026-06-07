#include "menu.h"
#include "input.h"
#include "utils.h"
#include <GLES3/gl3.h>
#include <cmath>

MenuScreen::MenuScreen()
    : mSelected(0), mAnimTime(0), mInputCooldown(0),
      mStartGame(false), mShowCredits(false), mQuit(false),
      mBgVAO(0), mBgVBO(0), mBgProg(0) {}

void MenuScreen::init() {
    mSelected = 0;
    mAnimTime = 0;
    mInputCooldown = 0.3f;
    mStartGame = mShowCredits = mQuit = false;
}

void MenuScreen::ensureBackground() {
    if (mBgVAO) return;
    float verts[] = { -1,-1, 1,-1, 1,1, -1,1 };
    glGenVertexArrays(1, &mBgVAO);
    glGenBuffers(1, &mBgVBO);
    glBindVertexArray(mBgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mBgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Shader très simple : couleur uniforme (bleu foncé)
    const char* vs = "#version 300 es\nlayout(location=0) in vec2 aPos;\nvoid main(){ gl_Position=vec4(aPos,0,1); }";
    const char* fs = "#version 300 es\nout vec4 fragColor;\nvoid main(){ fragColor=vec4(0.1,0.05,0.2,1.0); }";
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, nullptr);
    glCompileShader(v);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, nullptr);
    glCompileShader(f);
    mBgProg = glCreateProgram();
    glAttachShader(mBgProg, v);
    glAttachShader(mBgProg, f);
    glLinkProgram(mBgProg);
    glDeleteShader(v);
    glDeleteShader(f);
}

void MenuScreen::update(float dt) {
    mAnimTime += dt;
    if (mInputCooldown > 0) mInputCooldown -= dt;

    Input& inp = Input::get();
    if (mInputCooldown <= 0) {
        if (inp.isMenuUp()) {
            mSelected = (mSelected + 2) % 3;
            mInputCooldown = 0.22f;
        } else if (inp.isMenuDown()) {
            mSelected = (mSelected + 1) % 3;
            mInputCooldown = 0.22f;
        } else if (inp.isMenuConfirm()) {
            if (mSelected == 0) mStartGame = true;
            else if (mSelected == 1) mShowCredits = true;
            else mQuit = true;
            mInputCooldown = 0.35f;
        }
    }
}

void MenuScreen::render() {
    ensureBackground();
    glDisable(GL_DEPTH_TEST);
    glUseProgram(mBgProg);
    glBindVertexArray(mBgVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Dessiner trois rectangles colorés pour simuler les options
    // (sans utiliser la police)
    float w = 0.4f, h = 0.1f;
    float startY = 0.2f;
    float step = 0.15f;
    for (int i = 0; i < 3; ++i) {
        float y = startY - i * step;
        float r = (i == mSelected) ? 0.2f : 0.6f;
        float g = (i == mSelected) ? 0.6f : 0.3f;
        float b = (i == mSelected) ? 0.2f : 0.1f;
        // Quad pour l'option
        float rect[] = { -w, y,  w, y,  w, y+h, -w, y+h };
        static GLuint rectVBO = 0;
        if (!rectVBO) glGenBuffers(1, &rectVBO);
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glEnable(GL_DEPTH_TEST);
}

int MenuScreen::getSelection() const { return mSelected; }
bool MenuScreen::shouldStartGame() const { return mStartGame; }
bool MenuScreen::shouldShowCredits() const { return mShowCredits; }
bool MenuScreen::shouldQuit() const { return mQuit; }
void MenuScreen::clearActions() { mStartGame = mShowCredits = mQuit = false; }
