#include "camera.h"
#include <cmath>

Camera::Camera() : mDistance(9.0f), mAngleH(0.0f), mAngleV(0.45f) {}

void Camera::setTarget(const vec3& target) { mTarget = target; }

void Camera::update(float dt, const vec3& target, float lookX, float lookY) {
    mTarget = target;
    mAngleH += lookX * dt * 2.5f;
    mAngleV += lookY * dt * 1.5f;
    if (mAngleV < 0.15f) mAngleV = 0.15f;
    if (mAngleV > 1.1f) mAngleV = 1.1f;

    float cosV = cosf(mAngleV);
    mPos.x = mTarget.x + mDistance * cosV * sinf(mAngleH);
    mPos.z = mTarget.z + mDistance * cosV * cosf(mAngleH);
    mPos.y = mTarget.y + mDistance * sinf(mAngleV) + 1.5f;
}

mat4 Camera::getViewMatrix() const { return mat4::lookAt(mPos, mTarget + vec3(0, 0.8f, 0), vec3(0, 1, 0)); }
mat4 Camera::getProjectionMatrix(float aspect) const { return mat4::perspective(3.14159f / 3.2f, aspect, 0.1f, 120.0f); }
vec3 Camera::getPosition() const { return mPos; }
