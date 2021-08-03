#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_textureCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tileFactor;
layout(location = 6) in float a_uses3DTexture;
layout(location = 7) in float a_ambiantIntensity;
layout(location = 8) in float a_diffuseIntensity;
layout(location = 9) in float a_specularIntensity;
layout(location = 10) in float a_shininess;
layout(location = 11) in float a_ignoresLighting;

#define NUM_DIRECTIONAL_LIGHTS 8

uniform mat4 u_dirLightSpaceMatrices[NUM_DIRECTIONAL_LIGHTS];

uniform mat4 u_viewProjMatrix;
uniform int u_numDirLights;

out vec4 v_dirLightSpaceFragCoords[NUM_DIRECTIONAL_LIGHTS];
flat out int v_numDirLights;
out vec3 v_fragPos;
out vec3 v_textureCoords;
out vec3 v_normal;
out vec4 v_color;
flat out float v_texIndex;
out float v_tileFactor;
flat out float v_uses3DTexture;

flat out float v_ambiantIntensity;
flat out float v_diffuseIntensity;
flat out float v_specularIntensity;
flat out float v_shininess;
flat out float v_ignoresLighting;

void main()
{
    v_numDirLights = u_numDirLights;
    
    for (int i = 0; i < v_numDirLights && i < NUM_DIRECTIONAL_LIGHTS; i++)
    {
        v_dirLightSpaceFragCoords[i] = u_dirLightSpaceMatrices[i] * vec4(a_position, 1.0);
    }
	v_textureCoords = a_textureCoords;
	v_color = a_color;
	v_normal = a_normal;
	v_texIndex = a_texIndex;
	v_tileFactor = a_tileFactor;
	gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
	v_uses3DTexture = a_uses3DTexture;
    v_fragPos = a_position;

    v_ambiantIntensity = a_ambiantIntensity;
    v_diffuseIntensity = a_diffuseIntensity;
    v_specularIntensity = a_specularIntensity;
    v_shininess = a_shininess;
    v_ignoresLighting = a_ignoresLighting;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

#define NUM_DIRECTIONAL_LIGHTS 8
#define NUM_POINT_LIGHTS 8

uniform vec4 u_dirLightColors[NUM_DIRECTIONAL_LIGHTS];
uniform vec3 u_dirLightPos[NUM_DIRECTIONAL_LIGHTS];
uniform float u_lightRadius[NUM_DIRECTIONAL_LIGHTS];

uniform int u_numPointLights;
uniform vec4 u_pointLightColors[NUM_POINT_LIGHTS];
uniform vec3 u_pointLightPos[NUM_POINT_LIGHTS];
uniform float u_pointLightFarPlanes[NUM_POINT_LIGHTS];

in vec4 v_dirLightSpaceFragCoords[NUM_DIRECTIONAL_LIGHTS];

flat in int v_numDirLights;
in vec3 v_fragPos;
in vec3 v_textureCoords;
in vec3 v_normal;
in vec4 v_color;
flat in float v_texIndex;
in float v_tileFactor;
flat in float v_uses3DTexture;

flat in float v_ambiantIntensity;
flat in float v_diffuseIntensity;
flat in float v_specularIntensity;
flat in float v_shininess;
flat in float v_ignoresLighting;


uniform sampler2D[8] u_texture;
uniform samplerCube[8] u_cubeMap;

uniform sampler2D[8] u_shadow2D;
uniform samplerCube[8] u_cubeMapShadowMap;

uniform vec3 u_camPos;

uniform bool u_useShadows;

// array of offset direction for sampling for the point light shadow calculations
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculationDirectionalLight(int index)
{
    // perform perspective divide
    vec3 projCoords = v_dirLightSpaceFragCoords[index].xyz / v_dirLightSpaceFragCoords[index].w;

    float distance = length(u_dirLightPos[index] - projCoords);

    //if the fragment is farther than the radius of the light then it is in shadow
    if (distance > u_lightRadius[index])
    {
        return 1.0f;
    }

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadow2D[int(index)], projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_dirLightPos[index] - v_fragPos);
    float bias = 0.03f;
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow2D[int(index)], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow2D[int(index)], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}

vec3 CalculateAmbiant(vec4 baseColor)
{
    return v_ambiantIntensity * baseColor.xyz;
}

//returns the diffuse + specular color
vec3 CalculateDiffuseSpecular(vec4 baseColor, vec3 normal, vec3 lightPos, vec4 lightColor)
{
    vec3 lightDir = normalize(lightPos - v_fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = v_diffuseIntensity * diff * lightColor.xyz;

    vec3 viewDir = normalize(u_camPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), v_shininess); 
    vec3 specular =  v_specularIntensity * lightColor.xyz * spec;

    return diffuse + specular;
}

vec4 CalculateDirectionalLight(vec4 baseColor, int index)
{
    vec3 normal = normalize(v_normal);
    
    // ambient
    vec3 ambient = CalculateAmbiant(baseColor);
    
    //calculate both specular and diffuse lighting
    vec3 diffuseSpecular = CalculateDiffuseSpecular(baseColor, normal, 
        u_dirLightPos[index], u_dirLightColors[index]);

    // calculate shadow
    float shadow = ShadowCalculationDirectionalLight(index);                      
    vec3 lighting = (ambient + (1.0 - shadow) * diffuseSpecular) * baseColor.xyz;    
    
    return vec4(lighting, 0);
}

float ShadowCalculationPointLight(int index)
{
    vec3 fragToLight = v_fragPos - u_pointLightPos[index];
    
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0f;
    float bias = 0.15f;
    int samples = 20;
    float viewDistance = length(u_camPos - v_fragPos);
    float diskRadius = (1.0f + (viewDistance / u_pointLightFarPlanes[index])) / 25.0f;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(u_cubeMapShadowMap[index], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= u_pointLightFarPlanes[index];   // undo mapping [0, 1]
        if(currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    
    return shadow;
}


vec4 CalculatePointLight(vec4 baseColor, int index)
{
    vec3 normal = normalize(v_normal);
    
    // ambient
    vec3 ambient = CalculateAmbiant(baseColor);
    
    //calculate both specular and diffuse lighting
    vec3 diffuseSpecular = CalculateDiffuseSpecular(baseColor, normal, 
        u_dirLightPos[index], u_dirLightColors[index]);

    // calculate shadow
    float shadow = ShadowCalculationPointLight(index);                      
    vec3 lighting = (ambient + (1.0 - shadow) * diffuseSpecular) * baseColor.xyz;    
    
    return vec4(lighting, 0);
}

vec4 CalculateLighting(vec4 baseColor)
{
    vec4 result = vec4(0, 0, 0, 1);

    for (int i = 0; i < v_numDirLights; i++)
    {
        result += CalculateDirectionalLight(baseColor, i);
    }

    for (int i = 0; i < u_numPointLights; i++)
    {
        result += CalculatePointLight(baseColor, i);
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
    
    //calculate shadows only if the shadows are on 
    //and if the object does not ignore lighting
    if (u_useShadows && v_ignoresLighting == 0.0f)
    {
        color = CalculateLighting(baseColor);
    }
    else
    {
        color = baseColor;
    }
    
    /*
    vec3 fragToLight = v_fragPos - u_pointLightPos[0];
    float closestDepth = texture(u_cubeMapShadowMap[0], fragToLight).r;
    closestDepth *= u_pointLightFarPlanes[0];   // undo mapping [0, 1]
    color = vec4(closestDepth, 0, 0, 1);
    */
}