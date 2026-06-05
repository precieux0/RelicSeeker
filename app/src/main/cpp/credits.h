#pragma once
#include <string>
#include <vector>

struct CreditEntry {
    std::string role;
    std::string names;
    float r, g, b; // couleur du texte
};

class CreditsScreen {
public:
    CreditsScreen();
    void init();
    void update(float dt);
    void render();
    bool isFinished() const { return mFinished; }
    void shutdown();
private:
    std::vector<CreditEntry> mEntries;
    float mScrollY;
    bool mFinished;
    float mFadeAlpha;   // pour fondu à la fin
    float mTimer;
    GLuint mBackgroundTexture; // texture de fond dégradé
    GLuint mVAO, mVBO;         // pour fond plein écran
    void createBackgroundTexture();
    void drawBackground();
};
