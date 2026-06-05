#pragma once
#include <vector>
#include "player.h"
#include "enemy.h"

class World {
public:
    static World& get();
    void init();
    void update(float dt);
    void render(class Renderer& r);
    bool isGameOver() const;
    int getCurrentLevel() const;
private:
    Player mPlayer;
    std::vector<Enemy> mEnemies;
    std::vector<vec3> mKeysPos;
    bool mGameOver;
    int mLevel;
    float mLevelTimer;
    void loadLevel(int lvl);
    void checkCollisions();
};
