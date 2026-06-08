#pragma once
#ifndef DESKTOP_BUILD
#include <android/log.h>
#else
#include <cstdio>
#include <cstdlib>
#endif
#include <cmath>
#include <cstring>

#ifndef DESKTOP_BUILD
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Relic", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Relic", __VA_ARGS__)
#else
#define LOGI(...) do { printf("[Relic] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { fprintf(stderr, "[Relic] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif

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
    vec3 normalized() const { float l = length(); if (l < 1e-6f) return vec3(0,0,0); return *this * (1.0f / l); }
};

struct vec4 {
    float x, y, z, w;
    vec4() : x(0), y(0), z(0), w(0) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct mat4 {
    float m[16];
    mat4() { memset(m, 0, sizeof(m)); }
    static mat4 identity() {
        mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }
    static mat4 translate(const vec3& t) {
        mat4 r = identity();
        r.m[12] = t.x;
        r.m[13] = t.y;
        r.m[14] = t.z;
        return r;
    }
    static mat4 scale(const vec3& s) {
        mat4 r = identity();
        r.m[0] = s.x;
        r.m[5] = s.y;
        r.m[10] = s.z;
        return r;
    }
    static mat4 rotate(float angle, const vec3& axis) {
        vec3 n = axis.normalized();
        float c = cosf(angle);
        float s = sinf(angle);
        float t = 1.0f - c;
        mat4 r = identity();
        r.m[0] = t*n.x*n.x + c;
        r.m[1] = t*n.x*n.y + s*n.z;
        r.m[2] = t*n.x*n.z - s*n.y;
        r.m[4] = t*n.x*n.y - s*n.z;
        r.m[5] = t*n.y*n.y + c;
        r.m[6] = t*n.y*n.z + s*n.x;
        r.m[8] = t*n.x*n.z + s*n.y;
        r.m[9] = t*n.y*n.z - s*n.x;
        r.m[10] = t*n.z*n.z + c;
        return r;
    }
    static mat4 perspective(float fov, float aspect, float zNear, float zFar) {
        float tanHalfFov = tanf(fov * 0.5f);
        mat4 r;
        r.m[0] = 1.0f / (aspect * tanHalfFov);
        r.m[5] = 1.0f / tanHalfFov;
        r.m[10] = -(zFar + zNear) / (zFar - zNear);
        r.m[11] = -1.0f;
        r.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
        return r;
    }
    static mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
        vec3 f = (center - eye).normalized();
        vec3 s = f.cross(up).normalized();
        vec3 u = s.cross(f);
        mat4 r = identity();
        r.m[0] = s.x;
        r.m[4] = s.y;
        r.m[8] = s.z;
        r.m[1] = u.x;
        r.m[5] = u.y;
        r.m[9] = u.z;
        r.m[2] = -f.x;
        r.m[6] = -f.y;
        r.m[10] = -f.z;
        r.m[12] = -s.dot(eye);
        r.m[13] = -u.dot(eye);
        r.m[14] = f.dot(eye);
        return r;
    }
    mat4 operator*(const mat4& o) const {
        mat4 r;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float sum = 0.0f;
                for (int i = 0; i < 4; ++i) {
                    sum += m[i * 4 + row] * o.m[col * 4 + i];
                }
                r.m[col * 4 + row] = sum;
            }
        }
        return r;
    }
};
