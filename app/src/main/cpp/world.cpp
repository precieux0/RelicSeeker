#include "world.h"
#include "input.h"
#include "story.h"
#include "sound.h"
#include <algorithm>

World& World::get() { static World w; return w; }

void World::init() {
    mGameOver = false;
    mLevel = 1;
    loadLevel(mLevel);
}

void World::loadLevel(int lvl) {
    mEnemies.clear();
    mKeysPos.clear();
    if (lvl == 1) {
        mEnemies.emplace_back(vec3(3,0,4));
        mEnemies.emplace_back(vec3(-2,0,5));
        mKeysPos.push_back(vec3(5,0.5f, -3));
        Story::get().trigger("enter_tomb");
    } else if (lvl == 2) {
        mEnemies.emplace_back(vec3(0,0,6));
        mEnemies.emplace_back(vec3(4,0,2));
        mEnemies.emplace_back(vec3(-4,0,3));
        mKeysPos.push_back(vec3(-5,0.5f, -2));
        Story::get().trigger("deeper");
    } else {
        Story::get().trigger("final_boss");
        mEnemies.emplace_back(vec3(0,0,0)); // boss
    }
}

void World::update(float dt) {
    if (mGameOver) return;
    Input& inp = Input::get();
    mPlayer.update(dt, inp.isLeft(), inp.isRight(), inp.isForward(), inp.isBack(), inp.isJump());
    for (auto& e : mEnemies) e.update(dt, mPlayer.getPosition());
    checkCollisions();
    // Vérifier ramassage clé
    for (auto it = mKeysPos.begin(); it != mKeysPos.end(); ) {
        if ((*it - mPlayer.getPosition()).length() < 1.0f) {
            mPlayer.addKey();
            Sound::get().playPickup();
            it = mKeysPos.erase(it);
            Story::get().trigger("key_found");
        } else ++it;
    }
    // Transition niveau si toutes clés collectées et ennemis morts
    if (mKeysPos.empty() && std::all_of(mEnemies.begin(), mEnemies.end(), [](Enemy& e){ return !e.isAlive(); })) {
        mLevelTimer += dt;
        if (mLevelTimer > 2.0f) {
            mLevel++;
            if (mLevel <= 3) loadLevel(mLevel);
            else Story::get().trigger("game_win");
            mLevelTimer = 0;
        }
    } else mLevelTimer = 0;
    if (mPlayer.getHealth() <= 0) { mGameOver = true; Story::get().trigger("game_over"); }
}

void World::checkCollisions() {
    for (auto& e : mEnemies) {
        if (e.isAlive() && (e.getPosition() - mPlayer.getPosition()).length() < 1.2f) {
            mPlayer.takeDamage(20);
            Sound::get().playHit();
            e.kill(); // l'ennemi meurt après avoir touché (simplifié)
        }
    }
}

void World::render(Renderer& r) { /* appelle render sur les entités */ }
bool World::isGameOver() const { return mGameOver; }
int World::getCurrentLevel() const { return mLevel; }
