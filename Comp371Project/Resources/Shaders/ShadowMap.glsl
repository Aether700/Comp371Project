#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_ambient;
layout(location = 3) in vec3 a_diffuse;
layout(location = 4) in vec3 a_specular;
layout(location = 5) in float a_shininess;

uniform mat4 u_lightSpaceMatrix;

void main()
{
    gl_Position = u_lightSpaceMatrix * vec4(a_position, 1.0);
}
/*
#tipe geometry
#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_shadowMatrices[6];

//output of the geometry shader (will output more than one)
out vec4 v_fragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            v_fragPos = gl_in[i].gl_Position;
            gl_Position = u_shadowMatrices[face] * v_fragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
*/

#type fragment
#version 330 core

out vec4 color;

//take the output of the geometry shader as input
//in vec4 v_fragPos;

in vec4 gl_FragCoord;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main()
{
    /*
    vec3 fragCoord = vec3(v_fragPos.x, v_fragPos.y, v_fragPos.z);
    float lightDistance = length(fragCoord - u_lightPos);
    
    // map to [0,1] range by dividing by u_farPlane
    lightDistance = lightDistance / u_farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance; 
    color = vec4(vec3(lightDistance, lightDistance, lightDistance), 1);
    */

    //gl_FragDepth = v_fragPosLightSpace.z;
    gl_FragDepth = gl_FragCoord.z;
    color = vec4(gl_FragCoord.z, 0, 0, 1);
}