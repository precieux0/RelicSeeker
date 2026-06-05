#include "mesh.h"
#include "utils.h"
#include <cstring>

Mesh::Mesh() : mVAO(0), mVBO(0), mEBO(0), mIndexCount(0) {}

Mesh::~Mesh() {
    if (mVAO) glDeleteVertexArrays(1, &mVAO);
    if (mVBO) glDeleteBuffers(1, &mVBO);
    if (mEBO) glDeleteBuffers(1, &mEBO);
}

void Mesh::createCube() {
    // 8 sommets
    vec3 vertices[8] = {
        vec3(-0.5f, -0.5f, -0.5f),   // 0
        vec3( 0.5f, -0.5f, -0.5f),   // 1
        vec3( 0.5f, -0.5f,  0.5f),   // 2
        vec3(-0.5f, -0.5f,  0.5f),   // 3
        vec3(-0.5f,  0.5f, -0.5f),   // 4
        vec3( 0.5f,  0.5f, -0.5f),   // 5
        vec3( 0.5f,  0.5f,  0.5f),   // 6
        vec3(-0.5f,  0.5f,  0.5f)    // 7
    };

    // Normales pour chaque face (6 faces, chacune avec ses propres normales)
    vec3 normals[6] = {
        vec3(0, -1, 0),  // bas
        vec3(0,  1, 0),  // haut
        vec3(-1, 0, 0),  // gauche
        vec3( 1, 0, 0),  // droite
        vec3(0, 0, -1),  // avant
        vec3(0, 0,  1)   // arrière
    };

    // Indices pour 12 triangles (2 par face)
    GLushort indices[36] = {
        0,1,2, 0,2,3, // bas
        4,6,5, 4,7,6, // haut
        0,3,7, 0,7,4, // gauche
        1,5,6, 1,6,2, // droite
        0,4,5, 0,5,1, // avant
        3,2,6, 3,6,7  // arrière
    };

    mVertices.clear();
    mIndices.clear();

    // Construire les sommets avec leurs normales
    for (int i = 0; i < 36; ++i) {
        int idx = indices[i];
        int face = i / 6;          // 0 à 5
        Vertex v;
        v.pos = vertices[idx];
        v.normal = normals[face];
        mVertices.push_back(v);
        mIndices.push_back(i);
    }
    mIndexCount = 36;
    upload();
}

void Mesh::createPyramid() {
    // Sommets : base carrée et apex
    vec3 base[4] = {
        vec3(-0.5f, -0.5f, -0.5f),
        vec3( 0.5f, -0.5f, -0.5f),
        vec3( 0.5f, -0.5f,  0.5f),
        vec3(-0.5f, -0.5f,  0.5f)
    };
    vec3 apex(0.0f, 0.5f, 0.0f);

    mVertices.clear();
    mIndices.clear();

    // Face du bas (deux triangles)
    vec3 downNormal(0, -1, 0);
    mVertices.push_back({base[0], downNormal});
    mVertices.push_back({base[1], downNormal});
    mVertices.push_back({base[2], downNormal});
    mVertices.push_back({base[0], downNormal});
    mVertices.push_back({base[2], downNormal});
    mVertices.push_back({base[3], downNormal});
    for (int i = 0; i < 6; ++i) mIndices.push_back(i);

    // Faces latérales (triangles)
    int idx = 6;
    for (int i = 0; i < 4; ++i) {
        vec3 a = base[i];
        vec3 b = base[(i+1)%4];
        vec3 normal = (apex - a).cross(b - a).normalized();
        mVertices.push_back({a, normal});
        mVertices.push_back({b, normal});
        mVertices.push_back({apex, normal});
        mIndices.push_back(idx);
        mIndices.push_back(idx+1);
        mIndices.push_back(idx+2);
        idx += 3;
    }
    mIndexCount = (GLsizei)mIndices.size();
    upload();
}

void Mesh::upload() {
    if (mVertices.empty() || mIndices.empty()) return;

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLushort), mIndices.data(), GL_STATIC_DRAW);

    // Attribut position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    // Attribut normal (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    if (mVAO == 0 || mIndexCount == 0) return;
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
