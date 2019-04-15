#version 330 core
out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D normalTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int parFlag = 0;


/*in vec2 texCord;
in vec3 Normal;
in vec3 FragPos;*/

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){ 
    // number of depth layers
    const float numLayers = 60;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * 0.3; 
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = 1 - texture(texture1, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = 1 - texture(texture1, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1 - texture(texture1, prevTexCoords).r) - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);


    return finalTexCoords;  
    
 
} 

int MapInRange(float x, float in_min, float in_max, float out_min, float out_max){
    if(x < in_min) x = in_min;
	if(x > in_max) x = in_max;
	return int(((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min));
}

void main() {
    vec3 newPos;
    vec3 newNormal;
    //vec2 texCoords = texCord;
    vec2 texCoords = fs_in.TexCoords;
    //vec3 viewDir = normalize(viewPos-FragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    if(parFlag == 1)
        texCoords = parallaxMapping(fs_in.TexCoords, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
       discard;

    //newPos = vec3(FragPos.x, texture(texture1, texCoords).r*5.865, FragPos.z);
    newNormal = texture(normalTexture, vec2(texCoords.x, 0 + (1 - texCoords.y))).rgb;
    newNormal = normalize(newNormal * 2.0 - 1.0);
    
    vec3 norm = newNormal;
    vec3 lightColor = texture(texture2, texCoords).rgb;
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
 

    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    vec4 result = vec4(ambient + diffuse + specular, 1.0);
    FragColor = result;
}
