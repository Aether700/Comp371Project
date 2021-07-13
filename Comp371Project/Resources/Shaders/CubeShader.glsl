#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec4 v_color;

uniform mat4 u_viewProjMatrix;
uniform vec4 u_color;


void main()
{
    TexCoords = aPos;
    vec4 pos = u_viewProjMatrix * vec4(aPos, 1.0);
    gl_Position = pos;
    v_color = u_color;
}  

#type fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec4 v_color;

uniform samplerCube u_texture;

void main()
{    
    FragColor = texture(u_texture, TexCoords) * v_color;
}