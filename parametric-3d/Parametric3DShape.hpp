// from the private repo of Alp Cihan (https://github.com/alpcihan)
#ifndef PARAMETRIC3DSHAPE_HPP
#define PARAMETRIC3DSHAPE_HPP

#include "../glm/glm.hpp"
#include "../glm/gtc/constants.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include "../glm/gtx/transform.hpp"
#include "../glm/common.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "ParametricLine.hpp"

#include "Model3D.hpp"
#include <vector>
using namespace std;

class Parametric3DShape
{
public:
    static Model3D generate(glm::vec2 (*parametricLine)(float),
                            unsigned vertical,
                            unsigned rotation);

private:
    static glm::vec3 rotated(float t, float r, glm::vec2 (*parametricLine)(float));
    static unsigned int VHtoIndex(int v, int h, int max_v, int max_h);

    static vector<glm::vec3> fillPositions(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation);
    static vector<glm::vec3> fillNormals(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation);
    static vector<unsigned int> fillIndices(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation);
    static vector<glm::vec2> fillUvs(unsigned vertical, unsigned rotation);
    static glm::vec3 parametricSurface(glm::vec2 (*parametricLine)(float), float t, float r);

};

Model3D Parametric3DShape::generate(glm::vec2 (*parametricLine)(float),
                                    unsigned vertical,
                                    unsigned rotation)
{
    vector<glm::vec3> positions = fillPositions(parametricLine, vertical, rotation);
    vector<glm::vec3> normals = fillNormals(parametricLine, vertical, rotation);
    vector<glm::vec2> uvs = fillUvs(vertical, rotation);
    vector<unsigned int> indices = fillIndices(parametricLine, vertical, rotation);
    
    return Model3D(positions, normals, uvs, indices);
}

glm::vec3 Parametric3DShape::rotated(float t, float r, glm::vec2 (*parametricLine)(float))
{
    glm::vec3 point = glm::vec3((*parametricLine)(t), 0);

    return glm::rotateY(point, r * glm::two_pi<float>());
}

unsigned int Parametric3DShape::VHtoIndex(int v, int h, int max_v, int max_h)
{
    return (h % max_h) * max_v + v;
}

vector<glm::vec3> Parametric3DShape::fillPositions(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation)
{
    vector<glm::vec3> positions;

    // fill positions
    for (int h = 0; h < rotation; h++)
    {
        for (int v = 0; v < vertical; v++)
        {
            float t = (float)v / (vertical - 1);
            float r = (float)h / rotation;
            positions.push_back(rotated(t, r, parametricLine));
        }
    }

    return positions;
}

vector<unsigned int> Parametric3DShape::fillIndices(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation)
{
    vector<unsigned int> indices;

    for (int r = 0; r < rotation; r++)
    {
        for (int v = 0; v < vertical - 1; v++)
        {
            indices.push_back(VHtoIndex(v, r, vertical, rotation));
            indices.push_back(VHtoIndex(v + 1, r, vertical, rotation));
            indices.push_back(VHtoIndex(v, r + 1, vertical, rotation));

            indices.push_back(VHtoIndex(v, r + 1, vertical, rotation));
            indices.push_back(VHtoIndex(v + 1, r, vertical, rotation));
            indices.push_back(VHtoIndex(v + 1, r + 1, vertical, rotation));
        }
    }

    return indices;
}

vector<glm::vec3> Parametric3DShape::fillNormals(glm::vec2 (*parametricLine)(float), unsigned vertical, unsigned rotation)
{
    vector<glm::vec3> normals;

    for (int h = 0; h < rotation; ++h)
    {
        for (int v = 0; v < vertical; ++v)
        {
            const float t = (float)v / (vertical - 1);
            float r = (float)h / rotation;

            const float t_prev = (v-1.0) / (vertical - 1);
            const float t_next = (v+1.0) / (vertical - 1);
            
            glm::vec3 prev = glm::vec3(parametricLine(t_prev), 0);
            glm::vec3 next = glm::vec3(parametricLine(t_next), 0);

            glm::vec3 pv = prev-next;

            glm::vec3 norm = glm::rotateZ(pv, 90.0f);
            norm = glm::rotateY(norm, r*glm::two_pi<float>());
            norm = glm::normalize(norm);

            normals.push_back(norm);
        }
    }

    return normals;
}

vector<glm::vec2> Parametric3DShape::fillUvs(unsigned vertical, unsigned rotation)
{
    vector<glm::vec2> uvs;

    for (int h = 0; h < rotation; ++h)
    {
        for (int v = 0; v < vertical; ++v)
        {
            const float t = (float)v / (vertical - 1);
            float r = (float)h / rotation;
            uvs.push_back(glm::vec2(r,t));
        }
    }
    return uvs;
}

glm::vec3 Parametric3DShape::parametricSurface(glm::vec2 (*parametricLine)(float), float t, float r)
{
    glm::vec3 p = glm::vec3(parametricLine(t), 0.0f);
    return glm::rotateY(p, r * glm::two_pi<float>());
};

#endif
