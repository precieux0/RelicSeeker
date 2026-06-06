#include <android_native_app_glue.h>
#include "game.h"
#include "sound.h"
#include "utils.h"

void android_main(struct android_app* app) {
    // app_dummy() n'est plus nécessaire
    AAssetManager* assetManager = app->activity->assetManager;
    if (!Sound::get().init(assetManager)) {
        LOGE("Échec de l'initialisation du système audio");
    } else {
        Sound::get().loadSound("jump", "sounds/jump.wav");
        Sound::get().loadSound("pickup", "sounds/pickup.wav");
        Sound::get().loadSound("hit", "sounds/hit.wav");
        Sound::get().loadSound("credits_music", "sounds/credits_music.wav");
        LOGI("Sons chargés avec succès");
    }
    Game game(app);
    game.run();
}
