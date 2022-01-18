// from the private repo of Alp Cihan (https://github.com/alpcihan)
#pragma once

#include <vector>
#include "glm/glm.hpp"
using namespace std;

namespace CubeData
{
    vector<glm::vec3> positions = {
        // front
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, 1.0, 1.0),
        // back
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // right
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        // left
        glm::vec3(-1.0, 1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // top
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(-1.0, 1.0, 1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // bottom
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0)};

    vector<glm::vec3> normals = {
        // front
        glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0),
        // back
        glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0),
        // right
        glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
        // left
        glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0),
        // top
        glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
        // bottom
        glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)};

    vector<glm::vec2> uvs = {
        // front
        glm::vec2(0.666, 0.666),
        glm::vec2(0.666, 0.333),
        glm::vec2(0.333, 0.333),
        glm::vec2(0.333, 0.666),
        // back
        glm::vec2(0.666, 0.666),
        glm::vec2(0.666, 0.333),
        glm::vec2(0.333, 0.333),
        glm::vec2(0.333, 0.666),
        // right
        glm::vec2(0.666, 0.666),
        glm::vec2(0.666, 0.333),
        glm::vec2(0.333, 0.333),
        glm::vec2(0.333, 0.666),
        // left
        glm::vec2(0.666, 0.666),
        glm::vec2(0.666, 0.333),
        glm::vec2(0.333, 0.333),
        glm::vec2(0.333, 0.666),
        // top
        glm::vec2(0.666, 1.0),
        glm::vec2(0.666, 0.666),
        glm::vec2(0.333, 0.666),
        glm::vec2(0.333, 1.0),
        // bottom
        glm::vec2(0.666, 0.333),
        glm::vec2(0.666, 0.0),
        glm::vec2(0.333, 0.0),
        glm::vec2(0.333, 0.333),
    };

    vector<unsigned int> indices = {
        // front
        0,
        1,
        2,
        2,
        3,
        0,
        // back
        4,
        5,
        6,
        6,
        7,
        4,
        // right
        8,
        9,
        10,
        10,
        11,
        8,
        // left
        12,
        13,
        14,
        14,
        15,
        12,
        // top
        16,
        17,
        18,
        18,
        19,
        16,
        // bottom
        20,
        21,
        22,
        22,
        23,
        20,
    };
}
