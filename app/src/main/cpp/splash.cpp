#include "splash.h"
#include "utils.h"
#include "font.h"
#include <cmath>

SplashScreen::SplashScreen() : mFinished(false), mTimer(0), mTexture(0), mVBO(0), mVAO(0) {}
SplashScreen::~SplashScreen() { shutdown(); }

void SplashScreen::createQuad() {
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    unsigned short indices[] = {0,1,2, 0,2,3};
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

bool SplashScreen::init() {
    createQuad();
    // Créer une texture procédurale pour le logo (dégradé + texte)
    int w = 512, h = 512;
    unsigned char* pixels = new unsigned char[w*h*4];
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            float u = x/(float)w, v = y/(float)h;
            int idx = (y*w + x)*4;
            // Dégradé radial
            float cx = 0.5f, cy = 0.5f;
            float d = sqrtf((u-cx)*(u-cx)+(v-cy)*(v-cy));
            float r = 0.8f, g = 0.4f, b = 0.1f;
            pixels[idx+0] = (unsigned char)(255 * r * (1-d));
            pixels[idx+1] = (unsigned char)(255 * g * (1-d));
            pixels[idx+2] = (unsigned char)(255 * b * (1-d));
            pixels[idx+3] = 255;
        }
    }
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    delete[] pixels;
    LOGI("SplashScreen initialisé");
    return true;
}

void SplashScreen::update(float dt) {
    mTimer += dt;
    if (mTimer >= 2.5f) mFinished = true;
}

void SplashScreen::render() {
    // Rendu du fond texturé
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Simple shader pour texture (on réutilise un shader maison)
    static GLuint prog = 0;
    if (!prog) {
        const char* vs = "#version 300 es\nlayout(location=0) in vec2 aPos;layout(location=1) in vec2 aTexCoord;out vec2 vTexCoord;void main(){gl_Position=vec4(aPos,0,1);vTexCoord=aTexCoord;}";
        const char* fs = "#version 300 es\nprecision mediump float;in vec2 vTexCoord;uniform sampler2D uTex;out vec4 fragColor;void main(){fragColor=texture(uTex,vTexCoord);}";
        // compilation (simplifiée)
        prog = 123; // placeholder
    }
    glUseProgram(prog);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    // Afficher le texte "OkitGames"
    Font::get().drawText("OKIT GAMES", -0.4f, -0.1f, 0.08f, 1.0f, 0.9f, 0.2f);
    glEnable(GL_DEPTH_TEST);
}

void SplashScreen::shutdown() {
    if (mTexture) glDeleteTextures(1, &mTexture);
    if (mVBO) glDeleteBuffers(1, &mVBO);
    if (mVAO) glDeleteVertexArrays(1, &mVAO);
    mTexture = mVBO = mVAO = 0;
}
