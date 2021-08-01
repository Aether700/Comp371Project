#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_textureCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tileFactor;
layout(location = 6) in float a_uses3DTexture;

struct DirectionalLight
{
    vec4 color;
    vec3 position;
    mat4 lightSpaceMatrix;
    float shadowMapIndex;
    vec4 lightSpaceFragCoords;
};

#define NUM_DIRECTIONAL_LIGHTS 8

uniform mat4 u_viewProjMatrix;
uniform mat4 u_lightSpaceMatrix;
uniform DirectionalLight u_lightArr[NUM_DIRECTIONAL_LIGHTS];
uniform int u_numLights;

out DirectionalLight v_lightArr[NUM_DIRECTIONAL_LIGHTS];
flat out int v_numLights;
out vec4 v_lightSpaceCoords;
out vec3 v_fragPos;
out vec3 v_textureCoords;
out vec3 v_normal;
out vec4 v_color;
flat out float v_texIndex;
out float v_tileFactor;
flat out float v_uses3DTexture;

//done manually instead of in a loop since Opengl does not like 
//runtime variables being used to access memory
void CopyLightStructs()
{
    //index 0
    v_lightArr[0].position = u_lightArr[0].position;
    v_lightArr[0].shadowMapIndex = u_lightArr[0].shadowMapIndex;
    v_lightArr[0].lightSpaceFragCoords = u_lightArr[0].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[0].color = u_lightArr[0].color;
    
    //index 1
    v_lightArr[1].position = u_lightArr[1].position;
    v_lightArr[1].shadowMapIndex = u_lightArr[1].shadowMapIndex;
    v_lightArr[1].lightSpaceFragCoords = u_lightArr[1].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[1].color = u_lightArr[1].color;

    //index 2
    v_lightArr[2].position = u_lightArr[2].position;
    v_lightArr[2].shadowMapIndex = u_lightArr[2].shadowMapIndex;
    v_lightArr[2].lightSpaceFragCoords = u_lightArr[2].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[2].color = u_lightArr[2].color;

    //index 3
    v_lightArr[3].position = u_lightArr[3].position;
    v_lightArr[3].shadowMapIndex = u_lightArr[3].shadowMapIndex;
    v_lightArr[3].lightSpaceFragCoords = u_lightArr[3].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[3].color = u_lightArr[3].color;

    //index 4
    v_lightArr[4].position = u_lightArr[4].position;
    v_lightArr[4].shadowMapIndex = u_lightArr[4].shadowMapIndex;
    v_lightArr[4].lightSpaceFragCoords = u_lightArr[4].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[4].color = u_lightArr[4].color;

    //index 5
    v_lightArr[5].position = u_lightArr[5].position;
    v_lightArr[5].shadowMapIndex = u_lightArr[5].shadowMapIndex;
    v_lightArr[5].lightSpaceFragCoords = u_lightArr[5].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[5].color = u_lightArr[5].color;

    //index 6
    v_lightArr[6].position = u_lightArr[6].position;
    v_lightArr[6].shadowMapIndex = u_lightArr[6].shadowMapIndex;
    v_lightArr[6].lightSpaceFragCoords = u_lightArr[6].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[6].color = u_lightArr[6].color;

    //index 7
    v_lightArr[7].position = u_lightArr[7].position;
    v_lightArr[7].shadowMapIndex = u_lightArr[7].shadowMapIndex;
    v_lightArr[7].lightSpaceFragCoords = u_lightArr[7].lightSpaceMatrix * vec4(a_position, 1.0);
    v_lightArr[7].color = u_lightArr[7].color;
}

void main()
{
    v_numLights = u_numLights;
    CopyLightStructs();
    
    v_lightSpaceCoords = u_lightSpaceMatrix * vec4(a_position, 1.0);
	v_textureCoords = a_textureCoords;
	v_color = a_color;
	v_normal = a_normal;
	v_texIndex = a_texIndex;
	v_tileFactor = a_tileFactor;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
	v_uses3DTexture = a_uses3DTexture;
    v_fragPos = a_position;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

#define NUM_DIRECTIONAL_LIGHTS 8

struct DirectionalLight
{
    vec4 color;
    vec3 position;
    mat4 lightSpaceMatrix;
    float shadowMapIndex;
    vec4 lightSpaceFragCoords;
};

in DirectionalLight v_lightArr[NUM_DIRECTIONAL_LIGHTS];
flat in int v_numLights;
in vec4 v_lightSpaceCoords;
in vec3 v_fragPos;
in vec3 v_textureCoords;
in vec3 v_normal;
in vec4 v_color;
flat in float v_texIndex;
in float v_tileFactor;
flat in float v_uses3DTexture;

uniform sampler2D[8] u_texture;
uniform samplerCube[8] u_cubeMap;

uniform sampler2D[8] u_shadow2D;
uniform samplerCube[8] u_cubeMapShadowMap;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;

uniform bool u_useShadows;

float ShadowCalculationDirectionalLight(DirectionalLight light)
{
    // perform perspective divide
    vec3 projCoords = light.lightSpaceFragCoords.xyz / light.lightSpaceFragCoords.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadow2D[int(light.shadowMapIndex)], projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(light.position - v_fragPos);
    float bias = 0.03f;
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow2D[int(light.shadowMapIndex)], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow2D[int(light.shadowMapIndex)], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
    //return texture(u_shadow2D[int(light.shadowMapIndex)], projCoords.xy).r;
}

vec4 CalculateLighting(vec4 baseColor, DirectionalLight light)
{
    vec3 normal = normalize(v_normal);
    
    // ambient
    vec3 ambient = 0.3 * baseColor.xyz;
    
    // diffuse
    vec3 lightDir = normalize(light.position - v_fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.color.xyz;
    
    // specular
    vec3 viewDir = normalize(u_camPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular =  light.color.xyz * spec;
    
    // calculate shadow
    float shadow = ShadowCalculationDirectionalLight(light);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * baseColor.xyz;    
    
    return vec4(lighting, 1);
    //return vec4(shadow, 0, 0, 1);
}

void main()
{
	vec4 baseColor;
	if (v_uses3DTexture == 1.0f)
	{ 
		baseColor = texture(u_cubeMap[int(v_texIndex)], v_textureCoords * v_tileFactor) * v_color;
	}
	else
	{
		baseColor = texture(u_texture[int(v_texIndex)], v_textureCoords.xy * v_tileFactor) * v_color;
	}
    
    if (u_useShadows)
    {
        color = CalculateLighting(baseColor, v_lightArr[0]);
    }
    else
    {
        color = baseColor;
    }
}