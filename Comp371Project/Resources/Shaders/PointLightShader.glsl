#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;
layout(location = 3) in vec3 a_ambient;
layout(location = 4) in vec3 a_diffuse;
layout(location = 5) in vec3 a_specular;
layout(location = 6) in float a_shininess;

out vec2 TexCoords;

out VS_OUT {
    vec4 FragPosLightSpace;
    vec3 FragPos;
    vec2 textureCoords;
    vec3 Normal;
    vec4 color;
} vs_out;

uniform mat4 u_viewProjMatrix;
uniform mat4 u_lightSpaceMatrix;

void main()
{
    vs_out.FragPosLightSpace = u_lightSpaceMatrix * vec4(a_position, 1.0);
    //vs_out.FragPosLightSpace = vec4(a_position, 1.0);
    vs_out.FragPos = a_position;
    vs_out.Normal = a_normal;
    vs_out.color = vec4(0.3, 0.3, 0.85, 1);
    gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec4 FragPosLightSpace;
    vec3 FragPos;
    vec2 textureCoords;
    vec3 Normal;
    vec4 color;
} fs_in;

uniform samplerCube u_shadowMap;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

uniform float u_farPlane;

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - u_lightPos; 
    float closestDepth = texture(u_shadowMap, fragToLight).r;
    return closestDepth;
}

void main()
{
    vec3 color = fs_in.color.xyz;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
 
    // ambient
    vec3 ambient = 0.3 * color;
    
    // diffuse
    vec3 lightDir = normalize(u_lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(u_viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
  
    //FragColor = vec4(lighting, 1.0);
    FragColor = vec4(shadow, 0, 0, 1.0);
}