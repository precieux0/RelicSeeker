#include "game.h"
#include "renderer.h"
#include "input.h"
#include "world.h"
#include "story.h"
#include "sound.h"
#include "splash.h"
#include "credits.h"
#include "menu.h"
#include "font.h"
#include "utils.h"
#include "level.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <android/native_activity.h>

static Renderer renderer;
static SplashScreen splash;
static CreditsScreen credits;
static MenuScreen menu;
static bool sAssetsLoaded = false;
static bool sSoundLoaded = false;
static bool sInitFailed = false;
static std::string sLogFilePath;
static FILE* sLogFile = nullptr;

static void openLogFile(android_app* app) {
    if (sLogFile || !app || !app->activity) return;
    const char* dir = app->activity->internalDataPath;
    if (dir) {
        sLogFilePath = std::string(dir) + "/relic_seeker.log";
        sLogFile = fopen(sLogFilePath.c_str(), "w");
    }
    if (!sLogFile) {
        sLogFilePath = "/sdcard/relic_seeker.log";
        sLogFile = fopen(sLogFilePath.c_str(), "w");
    }
    if (sLogFile) {
        fprintf(sLogFile, "[relic] log opened at %s\n", sLogFilePath.c_str());
        fflush(sLogFile);
    }
}

#define FLOG(...) do { \
    LOGI(__VA_ARGS__); \
    if (sLogFile) { fprintf(sLogFile, __VA_ARGS__); fprintf(sLogFile, "\n"); fflush(sLogFile); } \
} while (0)

static void handleAppCmd(android_app* app, int32_t cmd) {
    Game* game = (Game*)app->userData;
    if (!game) return;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            FLOG("APP_CMD_INIT_WINDOW");
            if (app->window != nullptr) {
                if (!renderer.init(app->window)) {
                    FLOG("Renderer init FAILED");
                    sInitFailed = true;
                    game->mRunning = false;
                    return;
                }
                FLOG("Renderer init OK");
                if (!sAssetsLoaded) {
                    if (!Font::get().init()) {
                        FLOG("Font init FAILED");
                        sInitFailed = true;
                        game->mRunning = false;
                        return;
                    }
                    FLOG("Font init OK");
                    splash.init();
                    credits.init();
                    menu.init();
                    sAssetsLoaded = true;
                    FLOG("Assets loaded");
                }
                if (!sSoundLoaded) {
                    Sound& snd = Sound::get();
                    snd.init(app->activity->assetManager);
                    snd.loadSound("jump", "sounds/jump.wav");
                    snd.loadSound("pickup", "sounds/pickup.wav");
                    snd.loadSound("hit", "sounds/hit.wav");
                    snd.loadSound("credits_music", "sounds/credits_music.wav");
                    snd.loadSound("music_menu", "sounds/music_menu.wav");
                    for (int i = 1; i <= TOTAL_LEVELS; i++) {
                        char name[32], file[48];
                        snprintf(name, sizeof(name), "music_level%d", i);
                        snprintf(file, sizeof(file), "sounds/music_level%d.wav", i);
                        snd.loadSound(name, file);
                    }
                    Input::get().init(app);
                    sSoundLoaded = true;
                    FLOG("Sound + input init done");
                }
                game->mWindowReady = true;
                FLOG("Window READY");
            }
            break;
        case APP_CMD_TERM_WINDOW:
            FLOG("APP_CMD_TERM_WINDOW");
            game->mWindowReady = false;
            renderer.shutdown();
            // Les assets devront être ré-uploadés vers le nouveau contexte GL
            sAssetsLoaded = false;
            break;
        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_CONFIG_CHANGED:
            FLOG("Window resized / config changed");
            if (app->window) renderer.onResize(app->window);
            break;
        case APP_CMD_GAINED_FOCUS:
            game->mHasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            game->mHasFocus = false;
            break;
        case APP_CMD_DESTROY:
            FLOG("APP_CMD_DESTROY");
            game->mRunning = false;
            break;
        default:
            break;
    }
}

Game::Game(android_app* app)
    : mRunning(true), mWindowReady(false), mHasFocus(true),
      mApp(app), mState(STATE_SPLASH), mStateTimer(0) {
    mApp->userData = this;
    mApp->onAppCmd = handleAppCmd;
    openLogFile(app);
    FLOG("Game constructed");
}

void Game::changeState(GameState newState) {
    FLOG("changeState %d -> %d", (int)mState, (int)newState);
    mState = newState;
    mStateTimer = 0;
    if (newState == STATE_MENU) {
        Input::get().setUiMode(true);
        menu.init();
        Sound::get().switchMusic("music_menu", true);
    } else if (newState == STATE_PLAYING) {
        Input::get().setUiMode(false);
        World::get().resetGame();
    } else if (newState == STATE_CREDITS) {
        Input::get().setUiMode(true);
        Sound::get().switchMusic("credits_music", true);
        credits.init();
    }
}

void Game::run() {
    auto lastTime = std::chrono::steady_clock::now();
    while (mRunning) {
        handleEvents();
        if (!mRunning || sInitFailed) break;

        if (!mWindowReady) {
            // Petite attente passive pour ne pas brûler le CPU.
            ALooper_pollAll(16, nullptr, nullptr, nullptr);
            continue;
        }

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        if (dt > 0.1f) dt = 0.1f;
        lastTime = now;

        // IMPORTANT : on NE gate PAS update sur mHasFocus, sinon le splash peut
        // ne jamais finir si la focus event arrive trop tôt → écran noir éternel.
        update(dt);
        render();
    }
    if (sLogFile) { fclose(sLogFile); sLogFile = nullptr; }
}

int Game::handleEvents() {
    android_poll_source* source = nullptr;
    int ident, count = 0;
    // Poll non bloquant pour traiter tous les events en attente.
    while ((ident = ALooper_pollAll(0, nullptr, nullptr, (void**)&source)) >= 0) {
        if (source) { source->process(mApp, source); count++; }
        if (mApp->destroyRequested) { mRunning = false; break; }
    }
    if (mWindowReady) Input::get().update();
    return count;
}

void Game::update(float dt) {
    mStateTimer += dt;
    switch (mState) {
        case STATE_SPLASH:
            splash.update(dt);
            if (splash.isFinished()) changeState(STATE_MENU);
            break;
        case STATE_MENU:
            menu.update(dt);
            if (menu.shouldStartGame()) changeState(STATE_PLAYING);
            else if (menu.shouldShowCredits()) changeState(STATE_CREDITS);
            else if (menu.shouldQuit()) mRunning = false;
            break;
        case STATE_PLAYING:
            if (mHasFocus) {
                World::get().update(dt);
                Story::get().update(dt);
            }
            if (World::get().isGameOver()) changeState(STATE_MENU);
            else if (World::get().hasWon()) changeState(STATE_CREDITS);
            break;
        case STATE_CREDITS:
            credits.update(dt);
            if (credits.isFinished() || Input::get().isMenuConfirm()) changeState(STATE_MENU);
            break;
        default:
            break;
    }
}

void Game::render() {
    if (!mWindowReady) return;
    FLOG("Game::render state=%d windowReady=%d rendererInitialized=%d", (int)mState, (int)mWindowReady, renderer.isInitialized());
    if (!renderer.isInitialized()) {
        FLOG("Renderer not initialized before render, attempting recovery");
        if (mApp && mApp->window && renderer.init(mApp->window)) {
            FLOG("Renderer recovery init OK");
        } else {
            FLOG("Renderer recovery init FAILED");
            return;
        }
    }
    renderer.beginFrame();
    switch (mState) {
        case STATE_SPLASH:
            splash.render();
            break;
        case STATE_MENU:
            menu.render();
            break;
        case STATE_PLAYING:
            World::get().render(renderer);
            break;
        case STATE_CREDITS:
            credits.render();
            break;
        default:
            break;
    }
    renderer.endFrame();
}
