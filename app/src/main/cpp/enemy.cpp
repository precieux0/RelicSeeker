#include "enemy.h"
#include "renderer.h"
#include "mesh.h"
#include <cmath>

Enemy::Enemy(const vec3& pos, int type, float speed)
    : mPos(pos), mSpeed(speed), mAlive(true), mAngle(0), mType(type) {}

void Enemy::update(float dt, const vec3& player) {
    if (!mAlive) return;
    vec3 dir = player - mPos;
    float chaseRange = (mType == ENEMY_BOSS) ? 14.0f : 10.0f;
    if (dir.length() < chaseRange) {
        dir = dir.normalized();
        mPos = mPos + dir * mSpeed * dt;
    }
    mAngle += dt * (mType == ENEMY_SPIDER ? 5.0f : 2.5f);
}

void Enemy::render(Renderer& r) {
    // Le rendu est géré par World::render()
}

vec3 Enemy::getPosition() const { return mPos; }
bool Enemy::isAlive() const { return mAlive; }
void Enemy::kill() { mAlive = false; }
int Enemy::getType() const { return mType; }

vec3 Enemy::getColor() const {
    switch (mType) {
        case ENEMY_SPIDER: return vec3(0.15f, 0.12f, 0.10f);
        case ENEMY_MUMMY:  return vec3(0.65f, 0.58f, 0.35f);
        case ENEMY_BOSS:   return vec3(0.85f, 0.15f, 0.10f);
        default:           return vec3(0.55f, 0.20f, 0.18f);
    }
}

vec3 Enemy::getScale() const {
    switch (mType) {
        case ENEMY_SPIDER: return vec3(1.0f, 0.8f, 1.2f);
        case ENEMY_MUMMY:  return vec3(0.9f, 1.1f, 0.8f);
        case ENEMY_BOSS:   return vec3(1.3f, 1.3f, 1.3f);
        default:           return vec3(1.0f, 1.0f, 1.0f);
    }
}
