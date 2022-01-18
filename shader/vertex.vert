// from the private repo of Alp Cihan (https://github.com/alpcihan)
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 u_transform;
uniform mat4 u_pv;
uniform int is3D;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_uv;

void main()
{
    if (is3D == 5)
        gl_Position = u_pv * u_transform * vec4(aPos, 1.0);
    else if (is3D == 6)
        gl_Position = u_transform * vec4(aPos, 1.0);
    
    vertex_position = vec3(u_transform * vec4(aPos, 1.0));
    vertex_normal = vec3(u_transform * vec4(aNormal, 0.0));
    vertex_uv = aUv;
}
