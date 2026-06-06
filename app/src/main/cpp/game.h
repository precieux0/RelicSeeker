#pragma once
#include <android_native_app_glue.h>

enum GameState {
    STATE_SPLASH,
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_CREDITS,
    STATE_GAMEOVER
};

class Game {
public:
    Game(android_app* app);
    void run();

    bool mRunning;
    bool mWindowReady;
    bool mHasFocus;

private:
    android_app* mApp;
    GameState mState;
    float mStateTimer;
    int handleEvents();
    void update(float dt);
    void render();
    void changeState(GameState newState);
};
