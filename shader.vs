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
    texX = (curV.x + 15) / 30;
    texY = (curV.z + 15) / 30;

  
    gl_Position = projection * view * curV;
    
    Normal = aNormal;
    FragPos = vec3(curV);
    

    texCord = vec2(texX, texY);
} 
