#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"


SDL_Window* mainwindow;
SDL_GLContext maincontext;
float deltatime = 0.0f;
float lastframe = 0.0f;
bool running = true;
Camera camera(glm::vec3(0.0, 5.0f, 6.0f));

bool Init();

void InputProcess(SDL_Event event);

int main(int argc, char* args[]){
	std::cout<<"start"<<std::endl;
	Init();
	
	Shader ourShader("shader.vs", "shader.fs");
	float lines[] = {
		0.0f, 0.0f, 0.0f,
		5.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 5.0f, 0.0f, 

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 5.0f
	};

	std::vector<glm::vec3> vert;

	float curZ = -15.0;
	for(int i=0; i<300; i++){
		curZ+=0.1;
		float curX = -15.0;
		for(int j=0; j<300; j++){
			curX+=0.1f;
			vert.push_back(glm::vec3(-0.1f + curX, 0.5f, -0.1f + curZ));
		    vert.push_back(glm::vec3(-0.1f + curX, 0.5f, 0.1f + curZ));
		    vert.push_back(glm::vec3(0.1f + curX, 0.5f, -0.1f + curZ));

		    vert.push_back(glm::vec3(-0.1f + curX, 0.5f, 0.1f + curZ));
		    vert.push_back(glm::vec3(0.1f + curX, 0.5f, 0.1f + curZ));
		    vert.push_back(glm::vec3(0.1f + curX, 0.5f, -0.1f + curZ));	
		}
	}	

	

	unsigned int indices[] = {  // note that we start from 0!
	    0, 1, 2,    // first triangle
	    1, 3, 2      // second triangle
	};


	//------------------------------BUFFERS----------------------------------------//

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert), &vert[0], GL_STATIC_DRAW);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	//Position attribute
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	
	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	// set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
	
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("cores.jpg", &width, &height, &nrChannels, 0);
	
    if (data != NULL){
    	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
		GLenum error = glGetError();
		if(error != GL_NO_ERROR) std::cout << gluErrorString(error)<<std::endl;
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
   
	ourShader.use();
	//ourShader.setInt("texture2", 1);


	glEnable(GL_DEPTH_TEST);
	

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)800/(float)600, 0.1f, 100.0f);
	unsigned int projectLoc = glGetUniformLocation(ourShader.ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	while(running){
		if(glGetError() != GL_NO_ERROR){
			std::cout<<"Erro: "<<gluErrorString(glGetError())<<std::endl;
		}
		SDL_Event event;
		float currentframe = (float)SDL_GetTicks()/100;
		deltatime = currentframe - lastframe;
		lastframe = currentframe;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		ourShader.use();
	
		glm::mat4 view;
	
		view = camera.GetViewMatrix();
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


		glBindVertexArray(VAO);
	
		glm::mat4 model;
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		glDrawArrays(GL_TRIANGLES, 0, vert.size());
		

		float cameraSpeed = 2.5f * deltatime;


		SDL_GL_SwapWindow(mainwindow);

		InputProcess(event);
		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	SDL_Quit();

	return 0;

}

bool Init(){
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
	    std::cout << "Failed to init SDL\n";
	    return false;
	}

 
	// Create our window centered at 512x512 resolution
	mainwindow = SDL_CreateWindow(
	    "Incrivel janela", 
	    SDL_WINDOWPOS_CENTERED,
	    SDL_WINDOWPOS_CENTERED,
	    800,
	    600,
	    SDL_WINDOW_OPENGL
	);


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	maincontext = SDL_GL_CreateContext(mainwindow);
	if(maincontext == NULL){
		std::cout<<"failed to create context"<<std::endl;
		return false;
	}

	glewExperimental = GL_TRUE;

	GLenum status = glewInit();

	if(status != GLEW_OK){
		std::cout<<"failed to init glew"<<std::endl;
		return false;
	}

	return true;
}

void InputProcess(SDL_Event event){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT)
			running = false;
		
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.scancode == SDL_SCANCODE_UP)
				camera.ProcessKeyboard(FORWARD, deltatime);

			if(event.key.keysym.scancode == SDL_SCANCODE_DOWN)	
				camera.ProcessKeyboard(BACKWARD, deltatime);

			if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				camera.ProcessKeyboard(RIGHT, deltatime);

			if(event.key.keysym.scancode == SDL_SCANCODE_LEFT){
				camera.ProcessKeyboard(LEFT, deltatime);
			}	
			if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				running = false;
			if(event.key.keysym.scancode == SDL_SCANCODE_W)
				camera.ProcessMouseMovement(0, 10.0f);
			if(event.key.keysym.scancode == SDL_SCANCODE_S)
				camera.ProcessMouseMovement(0, -10.0f);
			if(event.key.keysym.scancode == SDL_SCANCODE_A)		
				camera.ProcessMouseMovement(10.0f, 0);
			if(event.key.keysym.scancode == SDL_SCANCODE_D)
				camera.ProcessMouseMovement(-10.0f, 0);	
		}
	}

}