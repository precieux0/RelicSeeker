#pragma once
#include "utils.h"

class Player {
public:
    Player();
    void update(float dt, bool left, bool right, bool forward, bool back, bool jump);
    void render(class Renderer& r);
    vec3 getPosition() const;
    int getHealth() const;
    void takeDamage(int dmg);
    void addKey();
    bool hasKey() const;
private:
    vec3 mPos, mVel;
    bool mGrounded;
    int mHealth;
    int mKeys;
    float mAnimTime;
};
