#type vertex
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
    gl_Position = vec4(a_position, 0, 1.0);
    v_texCoord = a_texCoord;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_shadowMap;

void main()
{           
    color = vec4(vec3(texture(u_shadowMap, v_texCoord).r), 1.0f);  
}