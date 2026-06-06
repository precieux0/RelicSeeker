#include "menu.h"
#include "font.h"
#include "input.h"
#include "utils.h"
#include <GLES3/gl3.h>
#include <cmath>
#include <string>

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
    float verts[] = { -1,-1,0,0, 1,-1,1,0, 1,1,1,1, -1,1,0,1 };
    glGenVertexArrays(1, &mBgVAO);
    glGenBuffers(1, &mBgVBO);
    glBindVertexArray(mBgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mBgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    const char* vs =
        "#version 300 es\n"
        "layout(location=0) in vec2 aPos;\n"
        "layout(location=1) in vec2 aTexCoord;\n"
        "out vec2 vUV;\n"
        "void main(){ gl_Position=vec4(aPos,0,1); vUV=aTexCoord; }";
    const char* fs =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 vUV;\n"
        "uniform float uTime;\n"
        "out vec4 fragColor;\n"
        "void main(){\n"
        "  vec3 top=vec3(0.05,0.08,0.14);\n"
        "  vec3 bot=vec3(0.18,0.12,0.06);\n"
        "  vec3 col=mix(bot,top,vUV.y);\n"
        "  float glow=0.15*sin(uTime*0.5+vUV.x*6.0)*sin(uTime*0.3+vUV.y*4.0);\n"
        "  col+=vec3(0.6,0.45,0.1)*glow;\n"
        "  fragColor=vec4(col,1.0);\n"
        "}";
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
    glUniform1f(glGetUniformLocation(mBgProg, "uTime"), mAnimTime);
    glBindVertexArray(mBgVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    float pulse = 0.5f + 0.5f * sinf(mAnimTime * 2.0f);
    Font::get().drawText("RELIC SEEKER", -0.55f, 0.55f, 0.09f, 1.0f, 0.85f, 0.25f, 1.0f);
    Font::get().drawText("ECHOES OF ETERNITY", -0.72f, 0.42f, 0.045f, 0.85f, 0.80f, 0.65f, 1.0f);

    const char* items[] = { "> NOUVELLE QUETE", "> CREDITS", "> QUITTER" };
    for (int i = 0; i < 3; ++i) {
        float y = 0.05f - i * 0.14f;
        bool sel = (i == mSelected);
        float r = sel ? 1.0f : 0.65f;
        float g = sel ? 0.85f : 0.60f;
        float b = sel ? 0.2f + 0.2f * pulse : 0.15f;
        float scale = sel ? 0.055f : 0.048f;
        std::string label = items[i];
        if (sel) label = std::string(">> ") + (items[i] + 2);
        Font::get().drawText(label, -0.55f, y, scale, r, g, b, 1.0f);
    }

    Font::get().drawText("Gauche: naviguer  |  Droite: valider", -0.78f, -0.82f, 0.032f, 0.5f, 0.5f, 0.5f, 0.9f);
    glEnable(GL_DEPTH_TEST);
}

int MenuScreen::getSelection() const { return mSelected; }
bool MenuScreen::shouldStartGame() const { return mStartGame; }
bool MenuScreen::shouldShowCredits() const { return mShowCredits; }
bool MenuScreen::shouldQuit() const { return mQuit; }
void MenuScreen::clearActions() { mStartGame = mShowCredits = mQuit = false; }
