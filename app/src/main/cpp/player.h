#pragma once
#include "utils.h"

class Player {
public:
    Player();
    void reset(const vec3& startPos);
    void update(float dt, bool left, bool right, bool forward, bool back, bool jump);
    void render(class Renderer& r);
    vec3 getPosition() const;
    int getHealth() const;
    int getMaxHealth() const;
    void takeDamage(int dmg);
    void addKey();
    int getKeys() const;
    bool hasKey() const;
private:
    vec3 mPos, mVel;
    bool mGrounded;
    int mHealth;
    int mMaxHealth;
    int mKeys;
    float mAnimTime;
};
