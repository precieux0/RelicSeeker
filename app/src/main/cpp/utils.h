#pragma once
#include <android/log.h>
#include <cmath>
#include <cstring>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Relic", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Relic", __VA_ARGS__)

struct vec3 {
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3 operator+(const vec3& v) const { return vec3(x+v.x, y+v.y, z+v.z); }
    vec3 operator-(const vec3& v) const { return vec3(x-v.x, y-v.y, z-v.z); }
    vec3 operator*(float s) const { return vec3(x*s, y*s, z*s); }
    vec3 operator/(float s) const { return vec3(x/s, y/s, z/s); }
    float dot(const vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    vec3 cross(const vec3& v) const { return vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
    float length() const { return sqrtf(x*x + y*y + z*z); }
    vec3 normalized() const { float l = length(); if (l<1e-6) return vec3(0,0,0); return *this * (1.0f/l); }
};

struct vec4 {
    float x, y, z, w;
    vec4() : x(0), y(0), z(0), w(1) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct mat4 {
    float m[16];
    mat4() { memset(m, 0, sizeof(m)); m[0]=m[5]=m[10]=m[15]=1.0f; }
    static mat4 identity() { return mat4(); }
    static mat4 perspective(float fov, float aspect, float near, float far);
    static mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up);
    static mat4 translate(const vec3& t);
    static mat4 rotate(float angle, const vec3& axis);
    static mat4 scale(const vec3& s);
    mat4 operator*(const mat4& other) const;
};

inline mat4 mat4::perspective(float fov, float aspect, float near, float far) {
    mat4 m;
    float tanHalfFov = tanf(fov * 0.5f);
    m.m[0] = 1.0f / (aspect * tanHalfFov);
    m.m[5] = 1.0f / tanHalfFov;
    m.m[10] = -(far + near) / (far - near);
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far * near) / (far - near);
    m.m[15] = 0.0f;
    return m;
}

inline mat4 mat4::lookAt(const vec3& eye, const vec3& target, const vec3& up) {
    vec3 f = (target - eye).normalized();
    vec3 r = f.cross(up).normalized();
    vec3 u = r.cross(f);
    mat4 m;
    m.m[0] = r.x; m.m[1] = u.x; m.m[2] = -f.x; m.m[3] = 0;
    m.m[4] = r.y; m.m[5] = u.y; m.m[6] = -f.y; m.m[7] = 0;
    m.m[8] = r.z; m.m[9] = u.z; m.m[10] = -f.z; m.m[11] = 0;
    m.m[12] = -r.dot(eye); m.m[13] = -u.dot(eye); m.m[14] = f.dot(eye); m.m[15] = 1;
    return m;
}

inline mat4 mat4::translate(const vec3& t) {
    mat4 m;
    m.m[12] = t.x; m.m[13] = t.y; m.m[14] = t.z;
    return m;
}

inline mat4 mat4::rotate(float angle, const vec3& axis) {
    float c = cosf(angle), s = sinf(angle);
    vec3 a = axis.normalized();
    mat4 m;
    m.m[0] = c + a.x*a.x*(1-c); m.m[1] = a.x*a.y*(1-c) + a.z*s; m.m[2] = a.x*a.z*(1-c) - a.y*s;
    m.m[4] = a.y*a.x*(1-c) - a.z*s; m.m[5] = c + a.y*a.y*(1-c); m.m[6] = a.y*a.z*(1-c) + a.x*s;
    m.m[8] = a.z*a.x*(1-c) + a.y*s; m.m[9] = a.z*a.y*(1-c) - a.x*s; m.m[10] = c + a.z*a.z*(1-c);
    return m;
}

inline mat4 mat4::scale(const vec3& s) {
    mat4 m;
    m.m[0] = s.x; m.m[5] = s.y; m.m[10] = s.z;
    return m;
}

inline mat4 mat4::operator*(const mat4& other) const {
    mat4 r;
    for (int i=0; i<4; i++) for (int j=0; j<4; j++) {
        r.m[i*4+j] = 0;
        for (int k=0; k<4; k++) r.m[i*4+j] += m[i*4+k] * other.m[k*4+j];
    }
    return r;
}
