#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_ambient;
layout(location = 3) in vec3 a_diffuse;
layout(location = 4) in vec3 a_specular;
layout(location = 5) in float a_shininess;

out vec2 TexCoords;

out VS_OUT {
    vec4 FragPosLightSpace;
    vec3 FragPos;
    vec3 Normal;
    vec4 color;
} vs_out;

uniform mat4 u_viewProjMatrix;
uniform mat4 u_lightSpaceMatrix;

void main()
{
    vs_out.FragPosLightSpace = u_lightSpaceMatrix * vec4(a_position, 1.0);
    vs_out.FragPos = a_position;
    vs_out.Normal = a_normal;
    vs_out.color = vec4(0.5, 0.5, 0.5, 1);
    gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec4 FragPosLightSpace;
    vec3 FragPos;
    vec3 Normal;
    vec4 color;
} fs_in;

const float PI = 3.1415926535897932384626433832795;

uniform sampler2D diffuseTexture;
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

/*
float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    
    // use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    
    //test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    /*
    // PCF
     float shadow = 0.0;
     float bias = 0.05; 
     float samples = 4.0;
     float offset = 0.1;
     for(float x = -offset; x < offset; x += offset / (samples * 0.5))
     {
       for(float y = -offset; y < offset; y += offset / (samples * 0.5))
       {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                closestDepth *= far_plane;   // Undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
       }
     }
     shadow /= (samples * samples * samples);

    /*
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    //display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}
*/

float spotlightScalar() {
    float theta = dot(normalize(fs_in.FragPos - lightPos), u_lightDir);
    
    if (theta > cos(radians(20.0f))) {
        return 1.0f;
    } else if(theta > cos(radians(30.0f))) {
        return (1.0f - cos(PI * (theta - cos(radians(30.0f)) / (cos(radians(20.0f) - cos(radians(30.0f)))) / 2.0f)));
    }

    return 0.0f;
}

void main()
{
    vec3 color = vec3(fs_in.color);
    vec3 lightColor = vec3(1, 0, 0);
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular =  lightColor * spec;// * vec4(specularMat, 1);  
        
    // calculate shadow
    float shadow = DirectionalShadow() /** spotlightScalar()*/;                      
    vec3 lighting = (ambient + shadow * (diffuse + specular));    

    //FragColor = vec4(lighting, 1);
    FragColor = vec4(shadow, 0, 0, 1);
}