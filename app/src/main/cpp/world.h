#include "mesh.h"

#pragma once
#include <vector>
#include "player.h"
#include "enemy.h"
#include "level.h"

class World {
public:
    static World& get();
    void init();
    void resetGame();
    void update(float dt);
    void render(class Renderer& r);
    bool isGameOver() const;
    bool hasWon() const;
    bool isPaused() const;
    void setPaused(bool p);
    void togglePause();
    int getCurrentLevel() const;
    const char* getLevelName() const;
    int getKeysNeeded() const;
    int getKeysCollected() const;
private:
    Player mPlayer;
    std::vector<Enemy> mEnemies;
    std::vector<vec3> mKeysPos;
    std::vector<LevelProp> mProps;
    bool mGameOver;
    bool mWon;
    bool mPaused;
    int mLevel;
    float mLevelTimer;
    float mAnimTime;
    int mKeysTotal;
    void loadLevel(int lvl);
    void checkCollisions();
    Mesh* meshForProp(PropType t);
};
