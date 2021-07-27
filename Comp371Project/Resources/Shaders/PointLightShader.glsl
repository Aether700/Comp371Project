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
    vec3 FragPos;
    vec3 Normal;
    vec4 color;
} vs_out;

uniform mat4 u_viewProjMatrix;

uniform bool u_reverseNormals;

void main()
{
    vs_out.FragPos = a_position;
    // a slight hack to make sure the outer large cube displays 
    //lighting from the 'inside' instead of the default 'outside'.
    //if(u_reverseNormals) 
    //    vs_out.Normal =  (-1.0 * a_normal);
    //else
        vs_out.Normal = a_normal;
    vs_out.color = vec4(1, 1, 1, 1);
    gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 color;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

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
    */

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
    */

    //display closestDepth as debug (to visualize depth cubemap)
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}
/*
void main()
{           
    vec3 color = vec3(fs_in.color);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1, 1, 1);
    
    // ambient
    vec3 ambient = 0.3 * color;
    
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1, 0, 0);
    //vec3 diffuse = normal;
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0f;                      
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    vec3 lighting = (ambient + diffuse + specular) * color;    
    
    FragColor = vec4(lighting, 1.0);

    //FragColor = texture(depthMap, fs_in.Normal);
}
*/

void main()
{
    vec4 lightColor = vec4(1, 0, 0, 1);
    // ambient
    vec4 ambient = 0.3f * lightColor;// * vec4(ambientMat, 1);
  	
    // diffuse 
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = lightColor * diff;// * vec4(diffuseMat, 1);
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewPos, reflectDir), 0.0), 32);
    vec4 specular = lightColor * spec;// * vec4(specularMat, 1);  
        
    //FragColor = (ambient + diffuse + specular) * vec4(1, 1, 1, 1);
    FragColor = (diffuse) * vec4(1, 1, 1, 1);
}