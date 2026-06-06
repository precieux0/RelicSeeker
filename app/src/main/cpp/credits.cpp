#include "credits.h"
#include "font.h"
#include "sound.h"
#include "utils.h"
#include <GLES3/gl3.h>

CreditsScreen::CreditsScreen() : mScrollY(1.2f), mFinished(false), mFadeAlpha(0), mTimer(0),
                                 mBackgroundTexture(0), mVAO(0), mVBO(0) {}

void CreditsScreen::createBackgroundTexture() {
    int w=512, h=512;
    unsigned char* pixels = new unsigned char[w*h*4];
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            float u=x/(float)w, v=y/(float)h;
            int idx=(y*w+x)*4;
            float r=0.2f+(1-v)*0.4f, g=0.1f+(1-v)*0.2f, b=0.3f+(1-v)*0.5f;
            if ((x%53==0 && y%37==0) || (x%89==0 && y%61==0)) r=g=b=1;
            pixels[idx]=(unsigned char)(r*255);
            pixels[idx+1]=(unsigned char)(g*255);
            pixels[idx+2]=(unsigned char)(b*255);
            pixels[idx+3]=255;
        }
    }
    glGenTextures(1, &mBackgroundTexture);
    glBindTexture(GL_TEXTURE_2D, mBackgroundTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    delete[] pixels;
    float vert[]={-1,-1,0,0, 1,-1,1,0, 1,1,1,1, -1,1,0,1};
    glGenVertexArrays(1,&mVAO);
    glGenBuffers(1,&mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER,mVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vert),vert,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void CreditsScreen::init() {
    createBackgroundTexture();
    mEntries = {
        {"RELIC SEEKER : ECHOES OF ETERNITY","",1,0.8f,0.2f},
        {"","",1,1,1},
        {"Developpe par","OKIT GAMES",1,0.6f,0},
        {"","",1,1,1},
        {"Direction technique","Okit Studio",0.8f,0.8f,1},
        {"Game Design","Okit Creator",0.8f,0.8f,1},
        {"Programmation 3D","Okit Dev",0.8f,0.8f,1},
        {"Art & UI","Okit Artist",0.8f,0.8f,1},
        {"Musique originale","Kevin MacLeod (incompetech.com)",0.7f,0.9f,0.7f},
        {"Effets sonores","Creazilla, Zapsplat, SoundBible",0.7f,0.9f,0.7f},
        {"","",1,1,1},
        {"Remerciements","",1,0.6f,0.2f},
        {"A tous les testeurs benevoles","",0.8f,0.8f,0.8f},
        {"A la communaute open-source","",0.8f,0.8f,0.8f},
        {"A nos familles","",0.8f,0.8f,0.8f},
        {"","",1,1,1},
        {"MERCI D'AVOIR JOUE !","",1,0.9f,0.1f},
        {"","",1,1,1},
        {"Appuyez sur l'ecran pour quitter","",0.5f,0.5f,0.5f}
    };
    mScrollY = 1.2f;
    mFinished = false;
    mFadeAlpha = 0;
    // Musique geree par Game::changeState
}

void CreditsScreen::drawBackground() {
    static GLuint prog=0;
    if (!prog) {
        const char* vs = "#version 300 es\nlayout(location=0) in vec2 aPos;layout(location=1) in vec2 aTexCoord;out vec2 vTexCoord;void main(){gl_Position=vec4(aPos,0,1);vTexCoord=aTexCoord;}";
        const char* fs = "#version 300 es\nprecision mediump float;in vec2 vTexCoord;uniform sampler2D uTex;out vec4 fragColor;void main(){fragColor=texture(uTex,vTexCoord);}";
        GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsId, 1, &vs, nullptr);
        glCompileShader(vsId);
        GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsId, 1, &fs, nullptr);
        glCompileShader(fsId);
        prog = glCreateProgram();
        glAttachShader(prog, vsId);
        glAttachShader(prog, fsId);
        glLinkProgram(prog);
        glDeleteShader(vsId);
        glDeleteShader(fsId);
    }
    glDisable(GL_DEPTH_TEST);
    glUseProgram(prog);
    glBindTexture(GL_TEXTURE_2D, mBackgroundTexture);
    glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void CreditsScreen::update(float dt) {
    mScrollY -= 0.25f*dt;
    if (mScrollY < -1.2f) {
        mFadeAlpha += dt*0.5f;
        if (mFadeAlpha >= 1.0f) {
            mFinished = true;
            Sound::get().stopCurrentMusic();
        }
    }
}

void CreditsScreen::render() {
    drawBackground();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float y=mScrollY, step=0.08f, cx=-0.7f;
    for (auto& e : mEntries) {
        float alpha=1;
        if (y>0.8f) alpha = 1 - (y-0.8f)*2;
        if (y<-0.8f) alpha = 1 - (-0.8f-y)*2;
        alpha = std::max(0.f, std::min(1.f, alpha));
        if (mFadeAlpha>0) alpha *= (1-mFadeAlpha);
        if (!e.role.empty()) {
            std::string line = e.role + (e.names.empty() ? "" : " : "+e.names);
            Font::get().drawText(line, cx, y, 0.045f, e.r, e.g, e.b, alpha);
        }
        y -= step;
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void CreditsScreen::shutdown() {
    if (mBackgroundTexture) glDeleteTextures(1, &mBackgroundTexture);
    if (mVBO) glDeleteBuffers(1, &mVBO);
    if (mVAO) glDeleteVertexArrays(1, &mVAO);
    mBackgroundTexture = mVBO = mVAO = 0;
    Sound::get().stopCurrentMusic();
}
