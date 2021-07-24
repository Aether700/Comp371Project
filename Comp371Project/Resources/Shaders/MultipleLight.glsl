#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_ambient;
layout(location = 3) in vec3 a_diffuse;
layout(location = 4) in vec3 a_specular;
layout(location = 5) in float a_shininess;

uniform mat4 u_viewProjMatrix;

out vec3 v_camPos;
out vec3 v_vertexPos;
out vec3 v_normal;
out vec3 v_ambient;
out vec3 v_diffuse;
out vec3 v_specular;
out float v_shininess;

void main()
{
    v_ambient = a_ambient;
    v_diffuse = a_diffuse;
    v_specular = a_specular;
    v_shininess= a_shininess;
    v_normal = a_normal;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
    v_vertexPos = a_position;
    v_camPos = vec3(u_viewProjMatrix[3][0], u_viewProjMatrix[3][1], u_viewProjMatrix[3][2]);
}


#type fragment
#version 330 core

struct PointLight
{
    vec4 color;
    vec3 position;
};

#define MAX_NUM_POINT_LIGHTS 5


layout(location = 0) out vec4 color;

uniform vec3 u_lightPos = vec3(0, 3, 0);
uniform PointLight u_pointLights[MAX_NUM_POINT_LIGHTS]; 
uniform int u_numLights = 0;

in vec3 v_camPos;
in vec3 v_vertexPos;
in vec3 v_normal;
in vec3 v_ambient;
in vec3 v_diffuse;
in vec3 v_specular;
in float v_shininess;

vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 ambientMat, 
    vec3 diffuseMat, vec3 specularMat, float shininessMat)
{
    // ambient
    vec4 ambient = 0.1f * light.color * vec4(ambientMat, 1);
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - v_vertexPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = light.color * diff * vec4(diffuseMat, 1);
    
    // specular
    vec3 viewDir = normalize(v_camPos - v_vertexPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(v_camPos, reflectDir), 0.0), shininessMat);
    vec4 specular = light.color * spec * vec4(specularMat, 1);  
        
    return ambient + diffuse + specular;

    /*
    // ambient
    float ambientStrength = 0.1;
    vec4 ambient = ambientStrength * light.color;
  	
    // diffuse 
    
    //in this case no need to normalize since the normal is a unit vector
    vec3 norm = normal; 
    vec3 lightDir = normalize(light.position - v_vertexPos);
    //take dot prod to get angleCoefficient and make sure it's > 0 with the max function
    float angleCoefficient = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = angleCoefficient * light.color;
            
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
    vec4 specular = specularStrength * spec * light.color;  

    //calculate the resulting color from the Phong lighting model
    return (ambient + diffuse + specular) * vec4(1.0f, 0.5f, 0.31f, 1);
    */
}

void main()
{
    vec4 result;
    if (u_numLights < MAX_NUM_POINT_LIGHTS)
    {
        for (int i = 0; i < u_numLights; i++)
        {
            result += CalculatePointLight(u_pointLights[i], v_normal, v_ambient, v_diffuse, v_specular, v_shininess);
        }
    }
    else
    {
        //default error color
        result = vec4(236 / 255, 0, 1, 1);
    }

    color = result;
}