#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <math.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"

void ClearOpenGLErrors() {
	GLenum error = glGetError();
	if(error != GL_NO_ERROR) {
		std::cout << gluErrorString(error) << std::endl;
	}
	
}
unsigned char *data1;
bool vertchangeup = false;
bool vertchangedown = false;
std::vector<unsigned char> normalsVec;
int VERTICES = 10;
std::vector<glm::vec3> vert;
std::vector<unsigned int> indices;
unsigned int EBO;
SDL_Window* mainwindow;
SDL_GLContext maincontext;
float deltatime = 0.0f;
float lastframe = 0.0f;
bool running = true;
glm::vec3 lightPos(-10.0f, 15.0f, 15.0f);
Camera camera(glm::vec3(0.0, 5.0f, 6.0f));
bool wireframe = false;

bool Init();

void InputProcess(SDL_Event event);

unsigned int createTerrain(const unsigned char* heightMap, int width);

unsigned int createAxes();

unsigned int createLamp();

void createNormalMap(const unsigned char* heightMap, int width);

template <typename T, typename I, typename O>
int MapInRange(T x, I in_min, I in_max, O out_min, O out_max)
{
	if(x < in_min) x = in_min;
	if(x > in_max) x = in_max;
	return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
bool sucessoo = Init();
Shader ourShader("shader.vs", "shader.fs");
Shader our2Shader("shader2.vs", "shader2.fs");
int main(int argc, char* args[]){
	std::cout<<"start"<<std::endl;
	
	
	
	ourShader.use();
	
	unsigned int VAO2 = createAxes();

	unsigned int lampVAO = createLamp();

	GLuint texture1, texture2, normalTexture;
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
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data1 = stbi_load("heightmap.jpg", &width, &height, &nrChannels, 0);

    if (data1 != NULL){
    	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
		GLenum error = glGetError();
		if(error != GL_NO_ERROR) std::cout << gluErrorString(error)<<std::endl;
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
	createNormalMap(data1, 1025);
	unsigned int VAO = createTerrain(data1, 1025);
    

	unsigned char *data;
	 // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("ps_texture_1k.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
		GLenum error = glGetError();
		if(error != GL_NO_ERROR) std::cout << gluErrorString(error)<<std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

	glGenTextures(1, &normalTexture);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	if (normalsVec.data() != NULL){
    	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1025, 1025, 0, GL_RGB, GL_UNSIGNED_BYTE, normalsVec.data());
        glGenerateMipmap(GL_TEXTURE_2D);
		GLenum error = glGetError();
		if(error != GL_NO_ERROR) std::cout << gluErrorString(error)<<std::endl;
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
   
	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setInt("texture2", 1);
	ourShader.setInt("normalTexture", 2);

	glEnable(GL_DEPTH_TEST);
	

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)800/(float)600, 0.1f, 100.0f);
	unsigned int projectLoc = glGetUniformLocation(ourShader.ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

	
	while(running){
		if(vertchangeup == true){
			VERTICES*=1.5;
			std::cout<<VERTICES<<std::endl;
			
			VAO = createTerrain(data1, 1025);
			vertchangeup = false;
		}
		if(vertchangedown == true){
			VERTICES/= 2;
			if(VERTICES <= 6)
				VERTICES = 6;
			VAO = createTerrain(data1, 1025);
			vertchangedown = false;
		}
		ClearOpenGLErrors();
		SDL_Event event;
		float currentframe = (float)SDL_GetTicks()/100;
		deltatime = currentframe - lastframe;
		lastframe = currentframe;

		glClearColor(0.2, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
	
		ourShader.use();
		glBindVertexArray(VAO);
		glm::mat4 view;
	
		view = camera.GetViewMatrix();
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		
	
		glm::mat4 model = glm::mat4(1.0);
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ClearOpenGLErrors();

		ourShader.setVec3("lightPos", lightPos); 

		ourShader.setVec3("viewPos", camera.Position); 

		//glDrawArrays(GL_POINTS, 0, vert.size());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		ClearOpenGLErrors();
		our2Shader.use();

		glBindVertexArray(VAO2);

		projectLoc = glGetUniformLocation(our2Shader.ID, "projection");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		viewLoc = glGetUniformLocation(our2Shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model2 = glm::mat4(1.0);
		/*model2 = glm::translate(model2, glm::vec3(0.0f, 3.0f, 0.0f));
		model2 = glm::scale(model2, glm::vec3(1.0f));*/
		modelLoc = glGetUniformLocation(our2Shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));


		glDrawArrays(GL_LINES, 0, 12);

		our2Shader.use();

		glBindVertexArray(lampVAO);

		projectLoc = glGetUniformLocation(our2Shader.ID, "projection");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		viewLoc = glGetUniformLocation(our2Shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		model2 = glm::mat4(1.0);
		model2 = glm::translate(model2, lightPos);
		model2 = glm::scale(model2, glm::vec3(1.0f));
		modelLoc = glGetUniformLocation(our2Shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		SDL_GL_SwapWindow(mainwindow);

		InputProcess(event);	
	}

	glDeleteVertexArrays(1, &VAO);
	

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
	    1366,
	    768,
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

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	return true;
}

void InputProcess(SDL_Event event){

	const Uint8* keyboardSnapshot = SDL_GetKeyboardState(NULL);

	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT)
			running = false;

		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x){
			vertchangeup = true;
			
		}	
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z){
			vertchangedown = true;
			
		}

	}

	if(keyboardSnapshot[SDL_SCANCODE_UP]) camera.ProcessKeyboard(FORWARD, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_DOWN]) camera.ProcessKeyboard(BACKWARD, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_RIGHT]) camera.ProcessKeyboard(RIGHT, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_LEFT]) camera.ProcessKeyboard(LEFT, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_W]) camera.ProcessMouseMovement(0, 10.0f, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_S]) camera.ProcessMouseMovement(0, -10.0f, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_D]) camera.ProcessMouseMovement(10.0f, 0, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_A]) camera.ProcessMouseMovement(-10.0f, 0, deltatime);
	if(keyboardSnapshot[SDL_SCANCODE_L]) lightPos.x += 0.2;
	if(keyboardSnapshot[SDL_SCANCODE_J]) lightPos.x	-= 0.2;
	if(keyboardSnapshot[SDL_SCANCODE_I]) lightPos.y	+= 0.2;
	if(keyboardSnapshot[SDL_SCANCODE_K]) lightPos.y	-= 0.2;
	if(keyboardSnapshot[SDL_SCANCODE_M]) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(keyboardSnapshot[SDL_SCANCODE_N]) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	

}

unsigned int createTerrain(const unsigned char* heightMap, int width){
	vert.clear();
	indices.clear();
	int nVerticesX, nVerticesY;
	nVerticesX = nVerticesY = VERTICES;
	
	float curZ = +14.8;
	for(int i=0; i<nVerticesX; i++){
		
		float curX = -14.8;
	
		for(int j=0; j<nVerticesY; j++){
			
			
			int heightMapX = MapInRange(curX, -14.8f, 14.8f, 0, 1024);
			int heightMapZ = MapInRange(curZ, -14.8f, 14.8f, 0, 1024);
			if(heightMapX > 1024){
				std::cout<<"Out of range = "<<heightMapX<<" "<<curX<<std::endl;
			}
			int vertexHeight = heightMap[((heightMapZ * width + heightMapX) * 3)];
			float curY = vertexHeight * 0.023;
			

			vert.push_back(glm::vec3(curX, curY, curZ));
			vert.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	
			curX+=29.8 / nVerticesX;	
		}
		curZ-=29.8/nVerticesY;
	}
	
	
	for(int i=0; i < (nVerticesX - 1) * (nVerticesY); i+=1){
		if(vert[i * 2].z != vert[(i * 2) + 2].z) continue;
		//if((i+2)%150==0) continue;
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+(nVerticesX));
		indices.push_back(i+1);
		indices.push_back(i + 1 + nVerticesX);
		indices.push_back(i + nVerticesX);
	}
	float nMin = 5465484, nMax = -50;
	for(int i=0; i<indices.size(); i += 3){
		glm::vec3 v1 = vert[indices[i+ 1] * 2] - vert[indices[i] * 2];
		glm::vec3 v2 = vert[indices[i+ 2] * 2] - vert[indices[i] * 2];
		//std::cout<<vert[indices[i+ 2] * 2].x<<" "<<vert[indices[i] * 2].x<<std::endl;

		glm::vec3 normal = glm::cross(v1, v2);

		vert[indices[i] * 2 + 1] += normal;
		vert[indices[i+1] * 2 + 1] += normal;
		vert[indices[i+2] * 2 + 1] += normal;
  
	}
	

	//for(int i=0; i< 900; i+=3) std::cout<<(unsigned int)normalsVec[i]<<" "<<(unsigned int)normalsVec[i+1]<<" "<<(unsigned int)normalsVec[i+2]<<std::endl;
	



	//------------------------------BUFFERS----------------------------------------//

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); 
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);


	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW); 

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); 

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1); 
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void createNormalMap(const unsigned char* heightMap, int width){
	int nVerticesX, nVerticesY;
	nVerticesX = nVerticesY = 1025;
	float curZ = +14.8;
	for(int i=0; i<nVerticesX; i++){
		
		float curX = -14.8;
	
		for(int j=0; j<nVerticesY; j++){
			
			
			int heightMapX = MapInRange(curX, -14.8f, 14.8f, 0, 1024);
			int heightMapZ = MapInRange(curZ, -14.8f, 14.8f, 0, 1024);
			if(heightMapX > 1024){
				std::cout<<"Out of range = "<<heightMapX<<" "<<curX<<std::endl;
			}
			int vertexHeight = heightMap[((heightMapZ * width + heightMapX) * 3)];
			float curY = vertexHeight * 0.023;
			

			vert.push_back(glm::vec3(curX, curY, curZ));
			vert.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	
			curX+=29.8 / nVerticesX;	
		}
		curZ-=29.8/nVerticesY;
	}
	std::cout<<"Create vertices"<<std::endl;
	
	
	for(int i=0; i < (nVerticesX - 1) * (nVerticesY); i+=1){
		if(vert[i * 2].z != vert[(i * 2) + 2].z) continue;
		//if((i+2)%150==0) continue;
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+(nVerticesX));
		indices.push_back(i+1);
		indices.push_back(i + 1 + nVerticesX);
		indices.push_back(i + nVerticesX);
	}
	float nMin = 5465484, nMax = -50;
	for(int i=0; i<indices.size(); i += 3){
		glm::vec3 v1 = vert[indices[i+ 1] * 2] - vert[indices[i] * 2];
		glm::vec3 v2 = vert[indices[i+ 2] * 2] - vert[indices[i] * 2];
		//std::cout<<vert[indices[i+ 2] * 2].x<<" "<<vert[indices[i] * 2].x<<std::endl;

		glm::vec3 normal = glm::cross(v1, v2);

		vert[indices[i] * 2 + 1] += normal;
		vert[indices[i+1] * 2 + 1] += normal;
		vert[indices[i+2] * 2 + 1] += normal;
		if(vert[indices[i] * 2 + 1].x > nMax) nMax = vert[indices[i] * 2 + 1].x;
		if(vert[indices[i] * 2 + 1].x < nMin) nMin = vert[indices[i] * 2 + 1].x;
		if(vert[indices[i] * 2 + 1].y > nMax) nMax = vert[indices[i] * 2 + 1].y;
		if(vert[indices[i] * 2 + 1].y < nMin) nMin = vert[indices[i] * 2 + 1].y;
		if(vert[indices[i] * 2 + 1].z > nMax) nMax = vert[indices[i] * 2 + 1].z;
		if(vert[indices[i] * 2 + 1].z < nMin) nMin = vert[indices[i] * 2 + 1].z;

		if(vert[indices[i+1] * 2 + 1].x > nMax) nMax = vert[indices[i+1] * 2 + 1].x;
		if(vert[indices[i+1] * 2 + 1].x < nMin) nMin = vert[indices[i+1] * 2 + 1].x;
		if(vert[indices[i+1] * 2 + 1].y > nMax) nMax = vert[indices[i+1] * 2 + 1].y;
		if(vert[indices[i+1] * 2 + 1].y < nMin) nMin = vert[indices[i+1] * 2 + 1].y;
		if(vert[indices[i+1] * 2 + 1].z > nMax) nMax = vert[indices[i+1] * 2 + 1].z;
		if(vert[indices[i+1] * 2 + 1].z < nMin) nMin = vert[indices[i+1] * 2 + 1].z;

		if(vert[indices[i+2] * 2 + 1].x > nMax) nMax = vert[indices[i+2] * 2 + 1].x;
		if(vert[indices[i+2] * 2 + 1].x < nMin) nMin = vert[indices[i+2] * 2 + 1].x;
		if(vert[indices[i+2] * 2 + 1].y > nMax) nMax = vert[indices[i+2] * 2 + 1].y;
		if(vert[indices[i+2] * 2 + 1].y < nMin) nMin = vert[indices[i+2] * 2 + 1].y;
		if(vert[indices[i+2] * 2 + 1].z > nMax) nMax = vert[indices[i+2] * 2 + 1].z;
		if(vert[indices[i+2] * 2 + 1].z < nMin) nMin = vert[indices[i+2] * 2 + 1].z;
  
	}
	
	float min = 15455, max = -800;
	for(int i=1; i<vert.size(); i += 2){
		//std::cout<<vert[i].x<<" "<<vert[i].y<<" "<<vert[i].z<<std::endl;
		normalsVec.push_back((unsigned char)MapInRange(vert[i].x, nMin, nMax, 0, 255));
		normalsVec.push_back((unsigned char)MapInRange(vert[i].y, nMin, nMax, 0, 255));
		normalsVec.push_back((unsigned char)MapInRange(vert[i].z, nMin, nMax, 0, 255));

  
	}
}

unsigned int createAxes(){
	float lines[] = {
	 	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 	100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	 	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 	0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f,

	 	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f 
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

unsigned int createLamp(){
	float vertices[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  

		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  

		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 

		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 

		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}