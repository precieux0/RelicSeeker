#include <android_native_app_glue.h>
#include "game.h"
#include "utils.h"

// IMPORTANT : ne PAS initialiser OpenSL ES ici. La fenêtre/contexte ne sont
// pas encore prêts et on évite la double initialisation (qui était l'une des
// causes de crash). Sound::init() est appelé une seule fois dans game.cpp
// lors de APP_CMD_INIT_WINDOW.
void android_main(struct android_app* app) {
    LOGI("android_main: démarrage de Relic Seeker");
    Game game(app);
    game.run();
    LOGI("android_main: sortie propre");
}
