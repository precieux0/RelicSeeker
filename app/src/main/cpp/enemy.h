#pragma once
#include "utils.h"

class Enemy {
public:
    Enemy(const vec3& pos);
    void update(float dt, const vec3& playerPos);
    void render(class Renderer& r);
    vec3 getPosition() const;
    bool isAlive() const;
    void kill();
private:
    vec3 mPos;
    float mSpeed;
    bool mAlive;
    float mAngle;
};
