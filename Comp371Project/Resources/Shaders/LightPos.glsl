#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;
layout(location = 3) in vec3 a_ambient;
layout(location = 4) in vec3 a_diffuse;
layout(location = 5) in vec3 a_specular;
layout(location = 6) in float a_shininess;


uniform mat4 u_viewProjMatrix;

void main()
{
    gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 330 core
out vec4 color;

void main()
{
    color = vec4(1, 0, 0, 1);
}