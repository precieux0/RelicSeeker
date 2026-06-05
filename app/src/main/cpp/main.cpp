#include <android_native_app_glue.h>
#include "game.h"
#include "sound.h"

void android_main(struct android_app* app) {
    app_dummy();

    // Initialisation du système audio
    AAssetManager* assetManager = app->activity->assetManager;
    if (!Sound::get().init(assetManager)) {
        LOGE("Échec de l'initialisation du système audio");
    } else {
        // Charger les sons (donnez-leur un nom logique)
        Sound::get().loadSound("jump", "sounds/jump.wav");
        Sound::get().loadSound("pickup", "sounds/pickup.wav");
        Sound::get().loadSound("hit", "sounds/hit.wav");
        LOGI("Sons chargés avec succès");
    }

    Game game(app);
    game.run();
}
