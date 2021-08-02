#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

uniform mat4 u_viewProjMatrix;

void main()
{
    vs_out.FragPos = aPos;
    vs_out.Normal = aNormal;
    gl_Position = u_viewProjMatrix * vec4(aPos, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform samplerCube u_shadowMap;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

uniform float u_farPlane;


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
    vec3 fragToLight = fragPos - u_lightPos;
    
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(u_shadowMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= u_farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, u_farPlane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(u_shadowMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= u_farPlane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(u_viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(u_shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= u_farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    FragColor = vec4(texture(u_shadowMap, fragToLight).r, 0, 0, 1.0);    
        
    return shadow;
}

void main()
{           
    vec3 color = vec3(0.3, 0.3, 0.85);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1, 1, 1);
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
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    vec3 lighting = (ambient + diffuse + specular) * color;    
    
    //FragColor = vec4(lighting, 1.0);
}