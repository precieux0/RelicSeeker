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
#include <android/asset_manager.h>
#include <sys/stat.h>
#include <unistd.h>

static Renderer renderer;
static SplashScreen splash;
static CreditsScreen credits;
static MenuScreen menu;
static bool sAssetsLoaded = false;
static bool sInitFailed = false;
static FILE* g_logFile = nullptr;

static void logToFile(const char* fmt, ...) {
    if (!g_logFile) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(g_logFile, fmt, args);
    fprintf(g_logFile, "\n");
    fflush(g_logFile);
    va_end(args);
}

static void handleAppCmd(android_app* app, int32_t cmd) {
    Game* game = (Game*)app->userData;
    if (!game) return;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            logToFile("APP_CMD_INIT_WINDOW");
            if (app->window != nullptr) {
                logToFile("window not null, initializing renderer");
                if (!renderer.init(app->window)) {
                    logToFile("Renderer init failed");
                    sInitFailed = true;
                    game->mRunning = false;
                    return;
                }
                logToFile("Renderer init OK");
                if (!sAssetsLoaded) {
                    logToFile("Loading assets");
                    if (!Font::get().init()) {
                        logToFile("Font init failed");
                        sInitFailed = true;
                        game->mRunning = false;
                        return;
                    }
                    splash.init();
                    credits.init();
                    menu.init();
                    sAssetsLoaded = true;
                    logToFile("Assets loaded");
                }
                static bool soundLoaded = false;
                if (!soundLoaded) {
                    Sound& snd = Sound::get();
                    snd.init(app->activity->assetManager);
                    snd.loadSound("jump", "sounds/jump.wav");
                    snd.loadSound("pickup", "sounds/pickup.wav");
                    snd.loadSound("hit", "sounds/hit.wav");
                    snd.loadSound("credits_music", "sounds/credits_music.wav");
                    snd.loadSound("music_menu", "sounds/music_menu.wav");
                    Input::get().init(app);
                    soundLoaded = true;
                    logToFile("Sound loaded");
                }
                game->mWindowReady = true;
                logToFile("Window ready set to true");
            }
            break;
        case APP_CMD_TERM_WINDOW:
            logToFile("APP_CMD_TERM_WINDOW");
            game->mWindowReady = false;
            renderer.shutdown();
            break;
        case APP_CMD_GAINED_FOCUS:
            game->mHasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            game->mHasFocus = false;
            break;
        case APP_CMD_DESTROY:
            logToFile("APP_CMD_DESTROY");
            game->mRunning = false;
            break;
        default:
            break;
    }
}

Game::Game(android_app* app)
    : mRunning(true), mWindowReady(false), mHasFocus(true),
      mApp(app), mState(STATE_SPLASH), mStateTimer(0) {
    // Ouvrir le fichier de log sur la carte SD
    char logPath[256];
    snprintf(logPath, sizeof(logPath), "/sdcard/relic_seeker.log");
    g_logFile = fopen(logPath, "w");
    logToFile("Game created, log file: %s", logPath);
    mApp->userData = this;
    mApp->onAppCmd = handleAppCmd;
}

void Game::changeState(GameState newState) {
    logToFile("changeState from %d to %d", mState, newState);
    mState = newState;
    mStateTimer = 0;
    if (newState == STATE_MENU) {
        Input::get().setUiMode(true);
        menu.init();
        menu.clearActions();
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
    logToFile("Game::run started");
    auto lastTime = std::chrono::steady_clock::now();
    while (mRunning) {
        if (!mWindowReady) {
            // Attendre passivement les événements (10 ms)
            ALooper_pollAll(10, nullptr, nullptr, nullptr);
            handleEvents(); // traiter les événements
            continue;
        }
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        if (dt > 0.1f) dt = 0.1f;
        lastTime = now;

        update(dt);
        render();
    }
    logToFile("Game::run finished");
    if (g_logFile) fclose(g_logFile);
}

int Game::handleEvents() {
    android_poll_source* source = nullptr;
    int ident, count = 0;
    while ((ident = ALooper_pollAll(0, nullptr, nullptr, (void**)&source)) >= 0) {
        if (source) { source->process(mApp, source); count++; }
    }
    if (mApp->destroyRequested) mRunning = false;
    if (mWindowReady) Input::get().update();
    return count;
}

void Game::update(float dt) {
    if (!mHasFocus) return;
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
            World::get().update(dt);
            Story::get().update(dt);
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
