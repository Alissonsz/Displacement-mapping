#version 330 core
out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D normalTexture;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
//uniform vec3 normalMap[1050625];

in vec2 texCord;
in vec3 Normal;
in vec3 FragPos;

int MapInRange(float x, float in_min, float in_max, float out_min, float out_max)
{
    if(x < in_min) x = in_min;
	if(x > in_max) x = in_max;
	return int(((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min));
}

void main() {
    //calcula a altura certa do fragmento para calculo da iluminacao
    /*vec3 iUp = vec3(FragPos.x, 0, FragPos.z + 0.1986);
    vec3 iRight = vec3(FragPos.x + 0.1986, 0, FragPos.z); 
    vec2 heightMapCord = vec2(MapInRange(FragPos.x, -14.8f, 14.8f, 0, 1), MapInRange(FragPos.z, -14.8f, 14.8f, 0, 1));
    vec4 newHeight = texture(texture1, heightMapCord);
    vec3 newCoord = vec3(FragPos.x, newHeight.r, FragPos.z);
    iUp.y = texture(texture1, vec2(heightMapCord.x, heightMapCord.y + 0.0044)).r ;
    iRight.y = texture(texture1, vec2(heightMapCord.x + 0.0044, heightMapCord.y)).r ;*/

    //vec3 newNormal = cross((iUp - newCoord), (iRight - newCoord));
    vec3 newPos = vec3(FragPos.x, texture(texture1, texCord).r*5.865, FragPos.z);
    vec3 newNormal = texture(normalTexture, vec2(texCord.x, 0 + (1 - texCord.y))).rgb;
    newNormal = normalize(newNormal * 2.0 - 1.0);
    
    //vec3 norm = normalize(Normal);
    vec3 norm = newNormal;
    vec3 lightDir = normalize(lightPos - newPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos-newPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
 

    float ambientStrength = 0.0;
    vec3 ambient = ambientStrength * lightColor;

    vec4 result = vec4(ambient + diffuse + specular, 1.0) * texture(texture2, texCord);
    FragColor = result;
}
