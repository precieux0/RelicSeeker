#pragma once
#include <GLES3/gl3.h>

class Shader {
public:
    Shader();
    ~Shader();
    bool load(const char* vertexSource, const char* fragmentSource);
    void use();
    GLuint getProgram() const;
    GLint getUniformLocation(const char* name);
    GLint getAttribLocation(const char* name);
private:
    GLuint mProgram;
};
