#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_ambient;
layout(location = 3) in vec3 a_diffuse;
layout(location = 4) in vec3 a_specular;
layout(location = 5) in float a_shininess;

void main()
{
    gl_Position = vec4(a_position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

//take the output of the geometry shader as input
in vec4 v_fragPos;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main()
{
    vec3 fragCoord = vec3(v_fragPos.x, v_fragPos.y, v_fragPos.z);
    float lightDistance = length(fragCoord - u_lightPos);
    
    // map to [0,1] range by dividing by u_farPlane
    lightDistance = lightDistance / u_farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance; 
    color = vec4(vec3(lightDistance, lightDistance, lightDistance), 1);
}