#type vertex
#version 430

in vec3 VertexPosition;

uniform mat4 u_viewProjMatrix;

void main()
{
	vec4 v= vec4(VertexPosition, 1);
	gl_Position= u_viewProjMatrix * v;
}

#type fragment
#version 430

out vec4 FragColor;

void main()
{
	FragColor = vec4(1, 1, 1, 1);
}