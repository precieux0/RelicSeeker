#include "splash.h"
#include "utils.h"
#include "font.h"
#include <cmath>

SplashScreen::SplashScreen() : mFinished(false), mTimer(0), mTexture(0), mVBO(0), mVAO(0) {}
SplashScreen::~SplashScreen() { shutdown(); }

void SplashScreen::createQuad() {
    float vertices[] = { -1,-1,0,0, 1,-1,1,0, 1,1,1,1, -1,1,0,1 };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

bool SplashScreen::init() {
    createQuad();
    LOGI("SplashScreen init - will draw red screen");
    return true;
}

void SplashScreen::update(float dt) {
    mTimer += dt;
    if (mTimer >= 2.5f) mFinished = true;
}

void SplashScreen::render() {
    glDisable(GL_DEPTH_TEST);
    // Remplir l'écran avec du rouge pour tester
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Afficher un texte (si la police fonctionne)
    Font::get().drawText("OKIT GAMES", -0.4f, -0.1f, 0.08f, 1,1,1,1);
    glEnable(GL_DEPTH_TEST);
}

void SplashScreen::shutdown() {
    if (mTexture) glDeleteTextures(1, &mTexture);
    if (mVBO) glDeleteBuffers(1, &mVBO);
    if (mVAO) glDeleteVertexArrays(1, &mVAO);
    mTexture = mVBO = mVAO = 0;
}
