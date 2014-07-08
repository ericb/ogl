#include <iostream>
#include <fstream>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include "config.h"
#include "util.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {

    if (SDL_Init(SDL_INIT_EVERYTHING | IMG_INIT_PNG) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    int width = 1280;
    int height = 720;

    SDL_Event event;
    
    // testing
    std::string p = assetPath();
    //std::cout << p + "testing" << std::endl;

    SDL_Window* window = SDL_CreateWindow("3DTestOpenGL", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, height, 
        SDL_WINDOW_OPENGL
    );

    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();

    GLuint programID = loadShaders("basic.vert", "basic.frag");

    // lets add some Model-View-Projection!
    glm::mat4 Projection = glm::perspective(70.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    
    glm::mat4 View = glm::lookAt(
        glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
    glm::mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    bool gameRunning = true;
    bool gamePaused = false;

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    const GLfloat vertices[] = {
        0.0f,  0.75f, 0.0f, // Vertex 1 (X, Y)    
        0.5f, -0.5f, 0.0f,  // Vertex 2 (X, Y)
        -0.5f, -0.5f, 0.0f  // Vertex 3 (X, Y)
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLint uniColor = glGetUniformLocation(programID, "c");
    glUseProgram(programID);

    while(gameRunning) {

        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameRunning = false;
                    break;

                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE) {
                        gameRunning = false;
                        break;
                    }
                    if(event.key.keysym.sym == SDLK_SPACE) {
                        gamePaused = gamePaused ? false : true;
                    }
            }
        }        
       
        if (!gamePaused) {
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(uniColor, 0.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            SDL_GL_SwapWindow(window);
        }      
    }

    glDeleteProgram(programID);
    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}