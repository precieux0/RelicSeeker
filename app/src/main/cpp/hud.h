#pragma once
#include "renderer.h"

class Hud {
public:
    static Hud& get();
    void draw(int health, int maxHealth, int level, const char* levelName,
              int keys, int keysNeeded, bool paused);
private:
    Hud() = default;
};
