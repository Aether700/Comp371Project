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

const float PI = 3.1415926535897932384626433832795;

uniform sampler2D u_shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;
uniform vec3 u_lightDir;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float DirectionalShadow()
{
    // this function returns 1.0 when the surface receives light, and 0.0 when it is in a shadow
    // perform perspective divide

    //normalized device coordinates
    vec3 ndc = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    
    // transform to [0,1] range
    ndc = ndc * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragment_position_light_space as coords)
    float closest_depth = texture(u_shadowMap, ndc.xy).r;
    return closest_depth;

    /*
    // get depth of current fragment from light's perspective
    float current_depth = ndc.z;

    // check whether current frag pos is in shadow
    float bias = 0.003f;  // bias applied in depth map: see shadow_vertex.glsl
    return ((current_depth - bias) < closest_depth) ? 1.0 : 0.0;
    */
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    /*
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // check whether current frag pos is in shadow
    float shadow = (currentDepth - 0.003f) > closestDepth  ? 1.0 : 0.0;

    return shadow;
    //return currentDepth;
    */
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
      
    return shadow;
}

/*
float spotlightScalar() {
    float theta = dot(normalize(fs_in.FragPos - lightPos), u_lightDir);
    
    if (theta > cos(radians(20.0f))) {
        return 1.0f;
    } else if(theta > cos(radians(30.0f))) {
        return (1.0f - cos(PI * (theta - cos(radians(30.0f)) / (cos(radians(20.0f) - cos(radians(30.0f)))) / 2.0f)));
    }

    return 0.0f;
}
*/

void main()
{
    /*
    vec3 color = vec3(fs_in.color);
    vec3 lightColor = vec3(1, 1, 1);
    // ambient
    vec3 ambient = 0.3f * lightColor;// * vec4(ambientMat, 1);
  	
    // diffuse 
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * diff;// * vec4(diffuseMat, 1);
    //vec4 diffuse = vec4(norm, 1);
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular =  lightColor * spec;// * vec4(specularMat, 1);  
        
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);//DirectionalShadow();                      
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular));    

    FragColor = vec4(lighting, 1);
    //FragColor = vec4(0, 0, 0, 1);
    //FragColor = vec4(shadow, 0, 0, 1);
    //FragColor = vec4(texture(u_shadowMap, fs_in.textureCoords).r, 0, 0, 1);
    */

    vec3 color = fs_in.color.xyz;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
    //vec3 fragColorTemp = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    //fragColorTemp = fragColorTemp * 0.5 + 0.5;
    //FragColor = vec4(fragColorTemp, 1);
}