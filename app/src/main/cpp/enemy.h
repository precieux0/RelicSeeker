#pragma once
#include "utils.h"

enum EnemyType {
    ENEMY_GUARDIAN = 0,
    ENEMY_SPIDER   = 1,
    ENEMY_MUMMY    = 2,
    ENEMY_BOSS     = 3
};

class Enemy {
public:
    Enemy(const vec3& pos, int type = ENEMY_GUARDIAN, float speed = 1.2f);
    void update(float dt, const vec3& playerPos);
    void render(class Renderer& r);
    vec3 getPosition() const;
    bool isAlive() const;
    void kill();
    int getType() const;
    vec3 getColor() const;
    vec3 getScale() const;
private:
    vec3 mPos;
    float mSpeed;
    bool mAlive;
    float mAngle;
    int mType;
};
