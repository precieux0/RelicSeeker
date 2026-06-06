#include "shader.h"
#include "utils.h"

static const char* defaultVertex =
    "#version 300 es\n"
    "uniform mat4 uMVP;\n"
    "uniform mat4 uModel;\n"
    "layout(location=0) in vec3 aPos;\n"
    "layout(location=1) in vec3 aNormal;\n"
    "out vec3 vNormal;\n"
    "out vec3 vWorldPos;\n"
    "void main() {\n"
    "  vec4 world = uModel * vec4(aPos, 1.0);\n"
    "  vWorldPos = world.xyz;\n"
    "  vNormal = mat3(uModel) * aNormal;\n"
    "  gl_Position = uMVP * vec4(aPos, 1.0);\n"
    "}\n";

static const char* defaultFragment =
    "#version 300 es\n"
    "precision mediump float;\n"
    "uniform vec3 uLightDir;\n"
    "uniform vec3 uColor;\n"
    "uniform vec3 uCameraPos;\n"
    "uniform vec3 uFogColor;\n"
    "uniform float uFogDensity;\n"
    "uniform float uEmissive;\n"
    "in vec3 vNormal;\n"
    "in vec3 vWorldPos;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "  vec3 N = normalize(vNormal);\n"
    "  vec3 L = normalize(uLightDir);\n"
    "  float diff = max(dot(N, L), 0.0);\n"
    "  float ambient = 0.22;\n"
    "  vec3 V = normalize(uCameraPos - vWorldPos);\n"
    "  vec3 H = normalize(L + V);\n"
    "  float spec = pow(max(dot(N, H), 0.0), 24.0) * 0.35;\n"
    "  vec3 lit = uColor * (ambient + diff * 0.78) + vec3(spec);\n"
    "  lit += uColor * uEmissive;\n"
    "  float dist = length(uCameraPos - vWorldPos);\n"
    "  float fog = 1.0 - exp(-uFogDensity * dist);\n"
    "  fog = clamp(fog, 0.0, 0.92);\n"
    "  fragColor = vec4(mix(lit, uFogColor, fog), 1.0);\n"
    "}\n";

Shader::Shader() : mProgram(0) {}
Shader::~Shader() { if (mProgram) glDeleteProgram(mProgram); }

bool Shader::load(const char* vs, const char* fs) {
    if (!vs) vs = defaultVertex;
    if (!fs) fs = defaultFragment;
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs, nullptr);
    glCompileShader(vertex);
    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) { char log[512]; glGetShaderInfoLog(vertex, 512, nullptr, log); LOGE("Vertex shader error: %s", log); return false; }
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) { char log[512]; glGetShaderInfoLog(fragment, 512, nullptr, log); LOGE("Fragment shader error: %s", log); return false; }
    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertex);
    glAttachShader(mProgram, fragment);
    glLinkProgram(mProgram);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success) { char log[512]; glGetProgramInfoLog(mProgram, 512, nullptr, log); LOGE("Link error: %s", log); return false; }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void Shader::use() { glUseProgram(mProgram); }
GLuint Shader::getProgram() const { return mProgram; }
GLint Shader::getUniformLocation(const char* name) { return glGetUniformLocation(mProgram, name); }
GLint Shader::getAttribLocation(const char* name) { return glGetAttribLocation(mProgram, name); }
