#include <android_native_app_glue.h>
#include "game.h"
#include "utils.h"

void android_main(struct android_app* app) {
    LOGI("=== Relic Seeker started ===");
    Game game(app);
    game.run();
    LOGI("=== Relic Seeker exited ===");
}
