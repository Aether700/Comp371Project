#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_viewProjMatrix;

out vec3 v_camPos;
out vec3 v_vertexPos;
out vec3 v_normal;

void main()
{
    v_normal = a_normal;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
    v_vertexPos = a_position;
    v_camPos = vec3(u_viewProjMatrix[3][0], u_viewProjMatrix[3][1], u_viewProjMatrix[3][2]);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 u_lightPos = vec3(0, 3, 0);

in vec3 v_camPos;
in vec3 v_vertexPos;
in vec3 v_normal;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1, 1, 1);
  	
    // diffuse 
    
    //in this case no need to normalize since the normal is a unit vector
    vec3 norm = v_normal; 
    vec3 lightDir = normalize(u_lightPos - v_vertexPos);
    //take dot prod to get angleCoefficient and make sure it's > 0 with the max function
    float angleCoefficient = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = angleCoefficient * vec3(1, 1, 1);
            
    // specular
    float specularStrength = 0.5; //define strength as setting

    //vector of sight between the camera and the current fragment
    vec3 viewDir = normalize(v_camPos - v_vertexPos);

    //calculate the reflection vector (take the inverse of lightDir since we need a vector 
    //from the light to the fragment, lightDir is the other way around)
    vec3 reflectDir = reflect(-lightDir, norm);  

    //spec is the "shiness value", the 32 is arbitrary and can be changed as a setting
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    //multiply by the light color so that it gets added to the surface we are rendering
    vec3 specular = specularStrength * spec * vec3(1, 1, 1);  

    //calculate the resulting color from the Phong lighting model
    vec3 result = (ambient + diffuse + specular) * vec3(1.0f, 0.5f, 0.31f);
    color = vec4(result, 1.0);
}