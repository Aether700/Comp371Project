#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_textureCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tileFactor;
layout(location = 6) in float a_uses3DTexture;

#define NUM_DIRECTIONAL_LIGHTS 8

uniform mat4 u_lightSpaceMatrices[NUM_DIRECTIONAL_LIGHTS];

uniform mat4 u_viewProjMatrix;
uniform int u_numLights;

out vec4 v_lightSpaceFragCoords[NUM_DIRECTIONAL_LIGHTS];
flat out int v_numLights;
out vec3 v_fragPos;
out vec3 v_textureCoords;
out vec3 v_normal;
out vec4 v_color;
flat out float v_texIndex;
out float v_tileFactor;
flat out float v_uses3DTexture;


void main()
{
    v_numLights = u_numLights;
    
    for (int i = 0; i < v_numLights && i < NUM_DIRECTIONAL_LIGHTS; i++)
    {
        v_lightSpaceFragCoords[i] = u_lightSpaceMatrices[i] * vec4(a_position, 1.0);
    }
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


uniform vec4 u_lightColors[NUM_DIRECTIONAL_LIGHTS];
uniform vec3 u_lightPos[NUM_DIRECTIONAL_LIGHTS];
uniform float u_shadowMapIndices[NUM_DIRECTIONAL_LIGHTS];
uniform float u_lightRadius[NUM_DIRECTIONAL_LIGHTS];

in vec4 v_lightSpaceFragCoords[NUM_DIRECTIONAL_LIGHTS];

flat in int v_numLights;
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

uniform vec3 u_camPos;

uniform bool u_useShadows;

float ShadowCalculationDirectionalLight(int index)
{
    // perform perspective divide
    vec3 projCoords = v_lightSpaceFragCoords[index].xyz / v_lightSpaceFragCoords[index].w;

    float distance = length(u_lightPos[index] - projCoords);

    //if the fragment is farther than the radius of the light then it is in shadow
    if (distance > u_lightRadius[index])
    {
        return 1.0f;
    }

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadow2D[int(u_shadowMapIndices[index])], projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos[index] - v_fragPos);
    float bias = 0.03f;
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow2D[int(u_shadowMapIndices[index])], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow2D[int(u_shadowMapIndices[index])], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}

vec4 CalculateDirectionalLight(vec4 baseColor, int index)
{
    vec3 normal = normalize(v_normal);
    
    // ambient
    vec3 ambient = 0.3f * baseColor.xyz;
    
    // diffuse
    vec3 lightDir = normalize(u_lightPos[index] - v_fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = 0.5f * diff * u_lightColors[index].xyz;
    

    // specular
    vec3 viewDir = normalize(u_camPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular =  u_lightColors[index].xyz * spec;


    // calculate shadow
    float shadow = ShadowCalculationDirectionalLight(index);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * baseColor.xyz;    
    
    return vec4(lighting, 0);
}

vec4 CalculateLighting(vec4 baseColor)
{
    vec4 result = vec4(0, 0, 0, 1);

    for (int i = 0; i < v_numLights; i++)
    {
        result += CalculateDirectionalLight(baseColor, i);
    }

    return result;
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
        color = CalculateLighting(baseColor);
    }
    else
    {
        color = baseColor;
    }
}