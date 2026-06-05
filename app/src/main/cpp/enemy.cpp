#include "enemy.h"
#include "renderer.h"
#include "mesh.h"
#include <cmath>

Enemy::Enemy(const vec3& pos) : mPos(pos), mSpeed(1.2f), mAlive(true), mAngle(0) {}
void Enemy::update(float dt, const vec3& player) {
    if (!mAlive) return;
    vec3 dir = player - mPos;
    if (dir.length() < 8.0f) {
        dir = dir.normalized();
        mPos = mPos + dir * mSpeed * dt;
    }
    mAngle += dt * 3.0f;
}
void Enemy::render(Renderer& r) { /* rendu d'une pyramide */ }
vec3 Enemy::getPosition() const { return mPos; }
bool Enemy::isAlive() const { return mAlive; }
void Enemy::kill() { mAlive = false; }
