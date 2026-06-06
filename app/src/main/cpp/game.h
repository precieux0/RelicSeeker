#pragma once
#include <android_native_app_glue.h>

enum GameState {
    STATE_SPLASH,
    STATE_PLAYING,
    STATE_CREDITS,
    STATE_GAMEOVER
};

class Game {
public:
    Game(android_app* app);
    void run();
    bool mRunning; // public pour que handleAppCmd puisse y accéder
private:
    android_app* mApp;
    GameState mState;
    float mStateTimer;
    void handleEvents();
    void update(float dt);
    void render();
    void changeState(GameState newState);
};
