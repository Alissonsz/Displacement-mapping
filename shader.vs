#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

//out vec3 ourColor;
uniform sampler2D texture1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 texCord;


void main(){ 

    vec4 curV;
    curV = model * vec4(aPos, 1.0);
    float texX, texY;
    texX=(curV.x + 15) / 30;
    texY=(curV.z + 15) / 30;
    curV.y = (texture(texture1, vec2(texX, texY)).x)*6;
    vec3 v1 =  vec3(curV.x+(0.02), (texture(texture1, vec2(texX+(0.02), texY)).x)*6, curV.z) - curV.xyz;
    vec3 v2 =  vec3(curV.x, (texture(texture1, vec2(texX, texY+(0.02))).x)*6, curV.z+(0.02)) - curV.xyz;
    gl_Position = projection * view * curV;
    float y =(curV.y + 2.0f)/4;
    if(y>=0.9 && y<=1) y=0.9f;
    //else y=0; 
    
    Normal = cross(v2, v1);
    //Normal = aNormal;
    FragPos = vec3(curV);
    

    texCord = vec2(texX, texY);
} 