#include "world.h"
#include "input.h"
#include "story.h"
#include "sound.h"
#include "renderer.h"
#include "mesh.h"
#include "camera.h"
#include "hud.h"
#include "level.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

static Mesh sCubeMesh;
static Mesh sPyramidMesh;
static Mesh sCylinderMesh;
static Mesh sPlaneMesh;
static bool sMeshesReady = false;

World& World::get() { static World w; return w; }

void World::init() {
    if (!sMeshesReady) {
        sCubeMesh.createCube();
        sPyramidMesh.createPyramid();
        sCylinderMesh.createCylinder(20);
        sPlaneMesh.createPlane();
        sMeshesReady = true;
    }
    resetGame();
}

void World::resetGame() {
    mGameOver = false;
    mWon = false;
    mPaused = false;
    mLevel = 1;
    mLevelTimer = 0;
    mAnimTime = 0;
    loadLevel(mLevel);
}

Mesh* World::meshForProp(PropType t) {
    switch (t) {
        case PROP_PILLAR:
        case PROP_COLUMN:
            return &sCylinderMesh;
        case PROP_TORCH:
            return &sCubeMesh;
        default:
            return &sCubeMesh;
    }
}

void World::loadLevel(int lvl) {
    const LevelDef& def = LevelData::get(lvl);
    mEnemies.clear();
    mKeysPos = def.keys;
    mProps = def.props;
    mKeysTotal = (int)def.keys.size();
    mPlayer.reset(def.playerStart);

    for (const auto& sp : def.enemies) {
        mEnemies.emplace_back(sp.pos, sp.type, sp.speed);
    }
    Story::get().trigger(def.storyEvent);
    mLevelTimer = 0;

    char musicName[32];
    snprintf(musicName, sizeof(musicName), "music_level%d", lvl);
    Sound::get().switchMusic(musicName, true);

    LOGI("Level loaded: %s", def.name.c_str());
}

void World::update(float dt) {
    if (mGameOver || mWon) return;

    Input& inp = Input::get();
    if (inp.isPause()) togglePause();
    if (mPaused) {
        if (inp.isAction()) mPaused = false;
        return;
    }

    mAnimTime += dt;
    mPlayer.update(dt, inp.isLeft(), inp.isRight(), inp.isForward(), inp.isBack(), inp.isJump());
    for (auto& e : mEnemies) e.update(dt, mPlayer.getPosition());
    checkCollisions();

    for (auto it = mKeysPos.begin(); it != mKeysPos.end(); ) {
        if ((*it - mPlayer.getPosition()).length() < 1.2f) {
            Sound::get().play("pickup");
            it = mKeysPos.erase(it);
            Story::get().trigger("key_found");
        } else ++it;
    }

    bool allDead = std::all_of(mEnemies.begin(), mEnemies.end(), [](Enemy& e){ return !e.isAlive(); });
    if (mKeysPos.empty() && allDead) {
        mLevelTimer += dt;
        if (mLevelTimer > 2.5f) {
            if (mLevel >= TOTAL_LEVELS) {
                mWon = true;
                Story::get().trigger("game_win");
            } else {
                mLevel++;
                loadLevel(mLevel);
                Story::get().trigger("level_complete");
            }
            mLevelTimer = 0;
        }
    } else {
        mLevelTimer = 0;
    }

    if (mPlayer.getHealth() <= 0) {
        mGameOver = true;
        Story::get().trigger("game_over");
    }
}

void World::checkCollisions() {
    for (auto& e : mEnemies) {
        if (!e.isAlive()) continue;
        float dist = (e.getPosition() - mPlayer.getPosition()).length();
        float hitRange = (e.getType() == ENEMY_BOSS) ? 1.8f : 1.3f;
        if (dist < hitRange) {
            int dmg = (e.getType() == ENEMY_BOSS) ? 30 : 15;
            mPlayer.takeDamage(dmg);
            Sound::get().play("hit");
            if (e.getType() != ENEMY_BOSS) e.kill();
        }
    }
}

void World::render(Renderer& r) {
    if (!sMeshesReady) return;

    const LevelDef& def = LevelData::get(mLevel);
    r.setEnvironment(def.skyColor, def.fogColor, def.fogDensity, def.lightDir);

    static Camera cam;
    cam.setTarget(mPlayer.getPosition());
    cam.update(0.016f, mPlayer.getPosition(), Input::get().getLookX(), Input::get().getLookY());
    r.setCamera(cam);

    mat4 ground = mat4::translate(vec3(0, -0.02f, 0)) * mat4::scale(vec3(28.0f, 0.08f, 28.0f));
    r.drawMesh(&sCubeMesh, ground, def.groundColor);

    for (const auto& p : mProps) {
        mat4 model = mat4::translate(p.pos) * mat4::scale(p.scale);
        r.drawMesh(meshForProp(p.type), model, p.color, p.emissive);
    }

    mat4 playerModel = mat4::translate(mPlayer.getPosition())
        * mat4::rotate(sinf(mAnimTime * 4.0f) * 0.05f, vec3(0, 1, 0))
        * mat4::scale(vec3(0.65f, 1.35f, 0.55f));
    r.drawMesh(&sCubeMesh, playerModel, vec3(0.18f, 0.50f, 0.85f));

    // Bandeau equipement (style aventuriere)
    mat4 band = mat4::translate(mPlayer.getPosition() + vec3(0, 0.5f, 0))
        * mat4::scale(vec3(0.7f, 0.15f, 0.7f));
    r.drawMesh(&sCubeMesh, band, vec3(0.55f, 0.40f, 0.22f));

    for (auto& e : mEnemies) {
        if (!e.isAlive()) continue;
        vec3 sc = e.getScale();
        mat4 enemyModel = mat4::translate(e.getPosition())
            * mat4::rotate(mAnimTime * 2.0f, vec3(0, 1, 0))
            * mat4::scale(sc);
        Mesh* mesh = (e.getType() == ENEMY_SPIDER) ? &sCubeMesh : &sPyramidMesh;
        if (e.getType() == ENEMY_MUMMY) mesh = &sCubeMesh;
        if (e.getType() == ENEMY_BOSS) mesh = &sPyramidMesh;
        r.drawMesh(mesh, enemyModel, e.getColor(), e.getType() == ENEMY_BOSS ? 0.15f : 0);
    }

    float spin = mAnimTime * 2.5f;
    for (const vec3& keyPos : mKeysPos) {
        float bob = sinf(mAnimTime * 3.0f + keyPos.x) * 0.15f;
        mat4 keyModel = mat4::translate(keyPos + vec3(0, bob, 0))
            * mat4::rotate(spin, vec3(0, 1, 0))
            * mat4::scale(vec3(0.3f, 0.5f, 0.3f));
        r.drawMesh(&sPyramidMesh, keyModel, vec3(1.0f, 0.82f, 0.12f), 0.35f);
    }

    Story::get().render(r);
    int collected = mKeysTotal - (int)mKeysPos.size();
    Hud::get().draw(mPlayer.getHealth(), mPlayer.getMaxHealth(), mLevel,
                    def.name.c_str(), collected, mKeysTotal, mPaused);
}

bool World::isGameOver() const { return mGameOver; }
bool World::hasWon() const { return mWon; }
bool World::isPaused() const { return mPaused; }
void World::setPaused(bool p) { mPaused = p; }
void World::togglePause() { mPaused = !mPaused; }
int World::getCurrentLevel() const { return mLevel; }
const char* World::getLevelName() const { return LevelData::get(mLevel).name.c_str(); }
int World::getKeysNeeded() const { return mKeysTotal; }
int World::getKeysCollected() const { return mKeysTotal - (int)mKeysPos.size(); }
