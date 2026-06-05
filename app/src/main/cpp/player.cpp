#include "player.h"
#include "renderer.h"
#include "mesh.h"
#include "utils.h"

static const float SPEED = 4.0f;
static const float GRAVITY = -12.0f;
static const float JUMP_POWER = 7.0f;

Player::Player() : mPos(0,1,0), mVel(0,0,0), mGrounded(true), mHealth(100), mKeys(0), mAnimTime(0) {}

void Player::update(float dt, bool left, bool right, bool forward, bool back, bool jump) {
    mAnimTime += dt;
    vec3 move(0,0,0);
    if (left) move.x -= 1;
    if (right) move.x += 1;
    if (forward) move.z -= 1;
    if (back) move.z += 1;
    if (move.x !=0 || move.z!=0) move = move.normalized();
    mVel.x = move.x * SPEED;
    mVel.z = move.z * SPEED;
    mPos.x += mVel.x * dt;
    mPos.z += mVel.z * dt;
    mVel.y += GRAVITY * dt;
    mPos.y += mVel.y * dt;
    mGrounded = (mPos.y <= 0.5f);
    if (mGrounded && mPos.y < 0.5f) {
        mPos.y = 0.5f;
        mVel.y = 0;
        if (jump) mVel.y = JUMP_POWER;
    }
    // Collision avec les limites
    mPos.x = fmaxf(-12.0f, fminf(12.0f, mPos.x));
    mPos.z = fmaxf(-12.0f, fminf(12.0f, mPos.z));
}

void Player::render(Renderer& r) {
    // Pour l'affichage, on dessine un cube (mesh créé globalement)
}

vec3 Player::getPosition() const { return mPos; }
int Player::getHealth() const { return mHealth; }
void Player::takeDamage(int dmg) { mHealth = fmaxf(0, mHealth - dmg); }
void Player::addKey() { mKeys++; }
bool Player::hasKey() const { return mKeys > 0; }
