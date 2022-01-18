// from the private repo of Alp Cihan (https://github.com/alpcihan)
#version 330 core

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_uv;

uniform int u_frame;
uniform sampler2D u_texture;
uniform vec3 u_light_pos;
uniform vec3 u_light_color;

out vec4 FragColor;

void main()
{
	// diff light
	vec3 norm = normalize(vertex_normal);
	vec3 lightDir = normalize(u_light_pos - vertex_position);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_light_color;

	// ambient light
	vec3 ambientLight = vec3(0.7,0.7,0.7);

	// texture
	vec3 surfaceColor = texture(u_texture, vertex_uv).rgb;
	
	// final color
	vec3 color = surfaceColor * (ambientLight + diffuse);

	FragColor = vec4(color, 1.0f);
}
