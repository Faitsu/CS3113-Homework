#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


//globals
ShaderProgram program;
SDL_Window* displayWindow;
bool gameIsRunning = true;


GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}


void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {

	//INITIALIZE
	//setting up 

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Meow!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	//variables

	float kirby_x = -4.5f;
	float kirby_y = -3.0f;

	float apple_rotate = 0.0f;
	float apple_rotate2 = 0.0f;
	float apple_rotate3 = 0.0f;

	float LastTicks = 0.0f;
	float counter = 0;

	float edgeLeft = -5.0;
	float edgeRight = 5.0;

	float buffer = 0.25f;

	bool moveRight = true;

	glm::mat4 viewMatrix, modelMatrix, appleMatrix, appleMatrix2, appleMatrix3, bgModelMatrix, projectionMatrix;

	GLuint BackgroundID;

	GLuint kirbyTextureID;
	GLuint kirbyTextureID2;

	GLuint appleTextureID;
	GLuint appleTextureID2;
	GLuint appleTextureID3;

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	appleMatrix = glm::mat4(1.0f);
	appleMatrix2 = glm::mat4(1.0f);
	appleMatrix3 = glm::mat4(1.0f);
	bgModelMatrix = glm::mat4(1.0f);


	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	//program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.5f, 0.8f, 0.9f, 0.1f);

	kirbyTextureID = LoadTexture("Kirby.png");
	kirbyTextureID2 = LoadTexture("Kirby2.png");

	appleTextureID = LoadTexture("Apple.png");
	appleTextureID2 = LoadTexture("Apple.png");
	appleTextureID3 = LoadTexture("Apple.png");

	BackgroundID = LoadTexture("Background.png");

	//setting up items 

	appleMatrix = glm::translate(appleMatrix, glm::vec3(3.0f, -2.25f, 0.0f));
	appleMatrix2 = glm::translate(appleMatrix2, glm::vec3(1.0f, 1.0f, 0.0f));
	appleMatrix3 = glm::translate(appleMatrix3, glm::vec3(-1.0f, -2.25f, 0.0f));

	bgModelMatrix = glm::scale(bgModelMatrix, glm::vec3(10.0f, 7.5f, 0.0f));


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (gameIsRunning) {
		
		//Procces Input
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				gameIsRunning = false;
			}
		}

		//Updating
		float tick = (float)SDL_GetTicks() / 1000.f;
		float deltaTime = tick - LastTicks;
		LastTicks = tick;

		if (counter <= 6) {
			kirby_y += 3.0f * 0.5f *deltaTime;
			if (moveRight) {
				kirby_x += 2.5f *deltaTime;
			}
			else {
				kirby_x -= 2.5f*deltaTime;
			}
		}
		else if (counter > 24) {
			counter = 0;

		}
		else {
			kirby_y -= 1.0f* 0.5f *deltaTime;
			if (moveRight) {
				kirby_x += 2.5f *deltaTime;
			}
			else {
				kirby_x -= 2.5f* deltaTime;
			}
		}

		if (kirby_x > (edgeRight - buffer)) {
			moveRight = false;
		}
		else if (kirby_x < (edgeLeft + buffer)) {
			moveRight = true;

		}

		apple_rotate += 0.25f * deltaTime;
		apple_rotate2 += 0.25f * deltaTime;
		apple_rotate3 += 0.25f * deltaTime;

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(kirby_x, kirby_y, 0.0f));
		appleMatrix = glm::rotate(appleMatrix, glm::radians(apple_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
		appleMatrix2 = glm::rotate(appleMatrix2, glm::radians(apple_rotate2), glm::vec3(0.0f, 0.0f, 1.0f));
		appleMatrix3 = glm::rotate(appleMatrix3, glm::radians(apple_rotate3), glm::vec3(0.0f, 0.0f, 1.0f));

		counter += .5;


		//rendering

		glClear(GL_COLOR_BUFFER_BIT);
		//bakcground
		program.SetModelMatrix(bgModelMatrix);
		float vertices3[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, BackgroundID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//apple1
		program.SetModelMatrix(appleMatrix);
		float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, appleTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//apple2

		program.SetModelMatrix(appleMatrix2);
		float vertices4[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords4[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, appleTextureID2);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//apple3

		program.SetModelMatrix(appleMatrix3);
		float vertices5[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords5[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords5);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, appleTextureID3);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//Kirby

		program.SetModelMatrix(modelMatrix);
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0, 0.0,1.0, 1.0,0.0, 0.0,0.0 };

		float fliptexCoords[] = { 1.0,1.0, 0.0,1.0, 0.0,0.0, 1.0,1.0, 0.0,0.0 , 1.0,0.0 };


		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		if (!moveRight) {
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, fliptexCoords);
		}
		else {
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		}
		glEnableVertexAttribArray(program.texCoordAttribute);
		if (counter <= 14) {
			glBindTexture(GL_TEXTURE_2D, kirbyTextureID);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, kirbyTextureID2);
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		SDL_GL_SwapWindow(displayWindow);


	}



	Shutdown();
	return 0;
}
