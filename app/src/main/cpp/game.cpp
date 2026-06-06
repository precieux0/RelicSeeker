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

static Renderer renderer;
static SplashScreen splash;
static CreditsScreen credits;
static bool sAssetsLoaded = false; // sons + monde chargés une seule fois

static void handleAppCmd(android_app* app, int32_t cmd) {
    Game* game = (Game*)app->userData;
    if (!game) return;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != nullptr) {
                LOGI("APP_CMD_INIT_WINDOW : initialisation du renderer");
                if (!renderer.init(app->window)) {
                    LOGE("Echec init renderer, abandon de la frame");
                    return;
                }
                // Ressources GL : font + splash + credits
                Font::get().init();
                splash.init();
                credits.init();

                // Charger sons + monde une seule fois
                if (!sAssetsLoaded) {
                    if (Sound::get().init(app->activity->assetManager)) {
                        Sound::get().loadSound("jump", "sounds/jump.wav");
                        Sound::get().loadSound("pickup", "sounds/pickup.wav");
                        Sound::get().loadSound("hit", "sounds/hit.wav");
                        Sound::get().loadSound("credits_music", "sounds/credits_music.wav");
                    } else {
                        LOGE("Sound init a échoué (on continue sans son)");
                    }
                    Input::get().init(app);
                    World::get().init();
                    sAssetsLoaded = true;
                }
                game->mWindowReady = true;
            }
            break;

        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW : libération du renderer");
            game->mWindowReady = false;
            renderer.shutdown();
            // NB : on ne touche pas au son ni au monde, on ne tue pas l'app.
            break;

        case APP_CMD_GAINED_FOCUS:
            game->mHasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            game->mHasFocus = false;
            break;

        case APP_CMD_DESTROY:
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
        handleEvents();
        if (!mRunning) break;

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        if (dt > 0.1f) dt = 0.1f; // clamp
        lastTime = now;

        // On ne rend / met à jour QUE si la fenêtre + GL sont prêts.
        if (mWindowReady) {
            update(dt);
            render();
        }
    }
}

void Game::handleEvents() {
    android_poll_source* source;
    // Si la fenêtre n'est pas prête, on bloque jusqu'à un événement (économise le CPU
    // ET surtout évite de boucler en rendant à vide -> crash GL).
    int timeoutMs = mWindowReady ? 0 : -1;
    int events;
    while (ALooper_pollAll(timeoutMs, nullptr, &events, (void**)&source) >= 0) {
        if (source) source->process(mApp, source);
        if (mApp->destroyRequested) {
            mRunning = false;
            return;
        }
        // Après le premier événement traité, on repasse en non-bloquant pour
        // permettre la boucle de rendu de tourner.
        timeoutMs = 0;
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
