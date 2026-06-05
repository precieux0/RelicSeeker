#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include "utils.h"  // pour vec3

struct Vertex {
    vec3 pos;
    vec3 normal;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void createCube();
    void createPyramid();
    void draw() const;  // dessine avec le shader déjà actif

private:
    GLuint mVAO, mVBO, mEBO;
    GLsizei mIndexCount;
    std::vector<Vertex> mVertices;
    std::vector<GLushort> mIndices;

    void upload();  // envoie les données vers la GPU
};
