#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;

//out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCord;

void main(){ 
    vec3 vec[4];
    vec[0] = vec3(-1.0, 1.0, 0.0);
    vec[1] = vec3(0.0, 1.0, 0.0);
    vec[2] = vec3(0.0, 0.0, 0.0);
    vec[3] = vec3(-1.0, 0.0, 0.0);

    vec4 curV;
    curV = model * vec4(aPos, 1.0);
    curV.y = sin(curV.x) + cos(curV.z);
    gl_Position = projection * view * curV;
    float y = (curV.y + 2.0f)/4;
    if(y>=0.9 && y<=1) y=0.9f;
    //else y=0; 

    texCord = vec2(0, y);
} 