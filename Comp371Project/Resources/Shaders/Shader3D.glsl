#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_textureCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tileFactor;
layout(location = 6) in float a_uses3DTexture;

uniform mat4 u_viewProjMatrix;

out vec3 v_textureCoords;
out vec3 v_normal;
out vec4 v_color;
out float v_texIndex;
out float v_tileFactor;
flat out float v_uses3DTexture;

void main()
{
	v_textureCoords = a_textureCoords;
	v_color = a_color;
	v_normal = a_normal;
	v_texIndex = a_texIndex;
	v_tileFactor = a_tileFactor;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
	v_uses3DTexture = a_uses3DTexture;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_textureCoords;
in vec3 v_normal;
in vec4 v_color;
in float v_texIndex;
in float v_tileFactor;
flat in float v_uses3DTexture;

uniform sampler2D[16] u_texture;
uniform samplerCube[16] u_cubeMap;



void main()
{
	if (v_uses3DTexture == 1.0f)
	{
		color = texture(u_cubeMap[int(v_texIndex)], v_textureCoords * v_tileFactor) * v_color;
	}
	else
	{
		color = texture(u_texture[int(v_texIndex)], v_textureCoords.xy * v_tileFactor) * v_color;
	}
}