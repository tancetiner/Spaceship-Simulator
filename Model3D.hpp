// from the private repo of Alp Cihan (https://github.com/alpcihan)
#ifndef VAO_HPP
#define VAO_HPP

#include <string>
#include <iostream>
#include <vector>
#include "GameObj3D.hpp"
#include "glm/glm.hpp"
#include "glad/glad.h"
using namespace std;

class Model3D
{
public:
    Model3D(const vector<glm::vec3> &positions, const vector<glm::vec3> &normals, const vector<glm::vec2> &uvs, const vector<unsigned int> &indices);
    ~Model3D();

    unsigned int getIndicesCount();

private:
    unsigned int vao = -1;
    unsigned int vbo_norm = -1, vbo_pos = -1, vbo_uv = -1, ibo = -1;
    unsigned int indicesCount = -1;
    vector<glm::vec3> positions;

    void create(const vector<glm::vec3> &positions, const vector<glm::vec3> &normals, const vector<glm::vec2> &uvs, const vector<unsigned int> &indices);
    void bind();

    friend class GameObj3D;
};

Model3D::Model3D(const vector<glm::vec3> &positions, const vector<glm::vec3> &normals, const vector<glm::vec2> &uvs, const vector<unsigned int> &indices)
    :positions(positions)
{
    create(positions, normals, uvs, indices);
    this->indicesCount = indices.size();
};

Model3D::~Model3D() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo_pos);
    glDeleteBuffers(1, &ibo);
}

unsigned int Model3D::getIndicesCount()
{
    return this->indicesCount;
}

void Model3D::create(const vector<glm::vec3> &positions, const vector<glm::vec3> &normals, const vector<glm::vec2> &uvs, const vector<unsigned int> &indices)
{
    // create vao
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // create attrib (positions)
    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // create attrib (normals)
    glGenBuffers(1, &vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // create attrib (uvs)
    glGenBuffers(2, &vbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * uvs.size(), uvs.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // create indices
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void Model3D::bind()
{
    glBindVertexArray(this->vao);
}

#endif
