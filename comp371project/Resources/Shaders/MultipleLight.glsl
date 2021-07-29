#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_ambient;
layout(location = 3) in vec3 a_diffuse;
layout(location = 4) in vec3 a_specular;
layout(location = 5) in float a_shininess;

uniform mat4 u_viewProjMatrix;
uniform mat4 u_lightSpaceMatrix;

out vec3 v_camPos;
out vec3 v_vertexPos;
out vec3 v_normal;
out vec3 v_ambient;
out vec3 v_diffuse;
out vec3 v_specular;
out float v_shininess;
out vec4 v_fragPosLightSpace;

void main()
{
    v_fragPosLightSpace = u_lightSpaceMatrix * vec4(a_position, 1);
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

uniform PointLight u_pointLights[MAX_NUM_POINT_LIGHTS]; 
uniform int u_numLights = 0;
uniform sampler2D u_shadowMap;

in vec3 v_camPos;
in vec3 v_vertexPos;
in vec3 v_normal;
in vec3 v_ambient;
in vec3 v_diffuse;
in vec3 v_specular;
in float v_shininess;
in vec4 v_fragPosLightSpace;

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

float ShadowCalculation(PointLight light, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = vec3(fragPosLightSpace.x, fragPosLightSpace.y, fragPosLightSpace.z) / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(light.position - v_vertexPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 c = vec3(1, 1, 1);
    vec3 normal = normalize(v_normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * c;
    // diffuse
    vec3 lightDir = normalize(u_pointLights[0].position - v_vertexPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(v_camPos - v_vertexPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(u_pointLights[0], v_fragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * c;    
    
    color = vec4(lighting, 1.0);
}

/*
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
*/