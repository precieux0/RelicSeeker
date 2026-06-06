#include "game.h"
#include "renderer.h"
#include "input.h"
#include "world.h"
#include "story.h"
#include "sound.h"
#include "splash.h"
#include "credits.h"
#include "font.h"
#include <chrono>

static Renderer renderer;
static SplashScreen splash;
static CreditsScreen credits;
static bool initOnce = false;

// Fonction statique pour le callback
static void handleAppCmd(android_app* app, int32_t cmd) {
    Game* game = (Game*)app->userData;
    if (!game) return;
    if (cmd == APP_CMD_INIT_WINDOW) {
        renderer.init(app->window);
        Input::get().init(app);
        Font::get().init();
        Sound::get().init(app->activity->assetManager);
        Sound::get().loadSound("jump", "sounds/jump.wav");
        Sound::get().loadSound("pickup", "sounds/pickup.wav");
        Sound::get().loadSound("hit", "sounds/hit.wav");
        Sound::get().loadSound("credits_music", "sounds/credits_music.wav");
        splash.init();
        credits.init();
        World::get().init();
        initOnce = true;
    }
    if (cmd == APP_CMD_TERM_WINDOW) {
        renderer.shutdown();
        Sound::get().shutdown();
        Font::get().shutdown();
        game->mRunning = false;
    }
}

Game::Game(android_app* app) : mApp(app), mRunning(true), mState(STATE_SPLASH), mStateTimer(0) {
    mApp->userData = this;
    mApp->onAppCmd = handleAppCmd;
}

void Game::changeState(GameState newState) {
    mState = newState;
    mStateTimer = 0;
    if (newState == STATE_SPLASH) {
        splash.init();
    } else if (newState == STATE_PLAYING) {
        World::get().init();
    } else if (newState == STATE_CREDITS) {
        credits.init();
    }
}

void Game::run() {
    auto lastTime = std::chrono::steady_clock::now();
    while (mRunning) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    android_poll_source* source;
    while (ALooper_pollAll(0, nullptr, nullptr, (void**)&source) >= 0) {
        if (source) source->process(mApp, source);
    }
    if (mApp->destroyRequested) mRunning = false;
    Input::get().update();
}

void Game::update(float dt) {
    switch (mState) {
        case STATE_SPLASH:
            splash.update(dt);
            if (splash.isFinished()) changeState(STATE_PLAYING);
            break;
        case STATE_PLAYING:
            World::get().update(dt);
            Story::get().update(dt);
            if (World::get().isGameOver()) changeState(STATE_CREDITS);
            else if (World::get().getCurrentLevel() > 3) changeState(STATE_CREDITS);
            break;
        case STATE_CREDITS:
            credits.update(dt);
            if (credits.isFinished() || Input::get().isAction()) mRunning = false;
            break;
        default: break;
    }
}

void Game::render() {
    renderer.beginFrame();
    switch (mState) {
        case STATE_SPLASH:
            splash.render();
            break;
        case STATE_PLAYING:
            World::get().render(renderer);
            Story::get().render(renderer);
            break;
        case STATE_CREDITS:
            credits.render();
            break;
        default: break;
    }
    renderer.endFrame();
}
