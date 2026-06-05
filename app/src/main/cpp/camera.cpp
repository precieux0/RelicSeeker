#include "camera.h"
#include <cmath>

Camera::Camera() : mDistance(8.0f), mAngleH(0.0f), mAngleV(0.5f) {}
void Camera::setTarget(const vec3& target) { mTarget = target; }
void Camera::update(float dt, const vec3& target) {
    mTarget = target;
    // contrôle avec le touché (simulé via input)
    // ici on fait varier les angles basés sur le déplacement du doigt (non implémenté pour l'exemple)
    mPos.x = mTarget.x + mDistance * cos(mAngleV) * sin(mAngleH);
    mPos.z = mTarget.z + mDistance * cos(mAngleV) * cos(mAngleH);
    mPos.y = mTarget.y + mDistance * sin(mAngleV);
}
mat4 Camera::getViewMatrix() const { return mat4::lookAt(mPos, mTarget, vec3(0,1,0)); }
mat4 Camera::getProjectionMatrix(float aspect) const { return mat4::perspective(3.14159f/3.0f, aspect, 0.1f, 100.0f); }
vec3 Camera::getPosition() const { return mPos; }
