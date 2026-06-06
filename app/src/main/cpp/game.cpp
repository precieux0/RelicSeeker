#include "game.h"
#include "renderer.h"
#include "input.h"
#include "world.h"
#include "story.h"
#include "sound.h"
#include "splash.h"
#include "credits.h"
#include "font.h"
#include "utils.h"
#include <chrono>
#include <thread>

static Renderer renderer;
static SplashScreen splash;
static CreditsScreen credits;
static bool sAssetsLoaded = false;

static void handleAppCmd(android_app* app, int32_t cmd) {
    Game* game = (Game*)app->userData;
    if (!game) return;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            if (app->window != nullptr) {
                if (!renderer.init(app->window)) {
                    LOGE("Renderer init failed");
                    return;
                }
                // Charger les ressources graphiques une fois
                if (!sAssetsLoaded) {
                    Font::get().init();
                    splash.init();
                    credits.init();
                    sAssetsLoaded = true;
                }
                // Son et Input une seule fois
                static bool soundLoaded = false;
                if (!soundLoaded) {
                    Sound::get().init(app->activity->assetManager);
                    Sound::get().loadSound("jump", "sounds/jump.wav");
                    Sound::get().loadSound("pickup", "sounds/pickup.wav");
                    Sound::get().loadSound("hit", "sounds/hit.wav");
                    Sound::get().loadSound("credits_music", "sounds/credits_music.wav");
                    Input::get().init(app);
                    World::get().init();
                    soundLoaded = true;
                }
                game->mWindowReady = true;
            }
            break;

        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            game->mWindowReady = false;
            renderer.shutdown();
            break;

        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
            game->mRunning = false;
            break;
    }
}

Game::Game(android_app* app)
    : mRunning(true), mWindowReady(false), mHasFocus(true),
      mApp(app), mState(STATE_SPLASH), mStateTimer(0) {
    mApp->userData = this;
    mApp->onAppCmd = handleAppCmd;
}

void Game::changeState(GameState newState) {
    mState = newState;
    mStateTimer = 0;
    if (newState == STATE_PLAYING) {
        World::get().init();
    }
}

void Game::run() {
    auto lastTime = std::chrono::steady_clock::now();
    while (mRunning) {
        // Attendre que la fenêtre soit prête
        while (!mWindowReady && mRunning) {
            handleEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (!mRunning) break;

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        if (dt > 0.1f) dt = 0.1f;
        lastTime = now;

        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    android_poll_source* source;
    int ident;
    while ((ident = ALooper_pollAll(0, nullptr, nullptr, (void**)&source)) >= 0) {
        if (source) source->process(mApp, source);
    }
    if (mApp->destroyRequested) {
        mRunning = false;
        return;
    }
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
    if (!mWindowReady) return;
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
