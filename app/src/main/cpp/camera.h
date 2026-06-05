#pragma once
#include "utils.h"

class Camera {
public:
    Camera();
    void update(float dt, const vec3& target);
    void setTarget(const vec3& target);
    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix(float aspect) const;
    vec3 getPosition() const;
private:
    vec3 mPos, mTarget;
    float mDistance, mAngleH, mAngleV;
};
