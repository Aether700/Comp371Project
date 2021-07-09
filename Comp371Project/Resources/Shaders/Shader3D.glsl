#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_texIndex;
layout(location = 4) in float a_tileFactor;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;
out vec3 v_normal;
out float v_texIndex;
out float v_tileFactor;

void main()
{
	v_color = a_color;
	v_normal = a_normal;
	v_texIndex = a_texIndex;
	v_tileFactor = a_tileFactor;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec3 v_normal;
in float v_texIndex;
in float v_tileFactor;

uniform samplerCube[32] u_texture;

void main()
{
    color = texture(u_texture[int(v_texIndex)], v_normal * v_tileFactor) * v_color;
}