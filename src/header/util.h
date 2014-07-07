#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "config.h"

/**
 *	Asset Path
 */
std::string assetPath(const std::string &subDir = ""){

#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif

    std::string baseRes = PROJECT_ROOT + std::string("/assets");
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

std::string readShader(const char* name) {
    std::string path = assetPath() + "/shader/" + name;
    std::string line;
    std::string content;
    std::fstream fh;
    fh.open(path);
    while(getline(fh, line)) {
        content += line + "\n";
    }
    fh.close();
    return content + '\0';
}

GLuint loadShaders(const char *vertex_path, const char *fragment_path) {

    glewExperimental = GL_TRUE;
    glewInit();

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_path);
    char const * vertexShader = readShader(vertex_path).c_str();
    glShaderSource(VertexShaderID, 1, &vertexShader , NULL);
    glCompileShader(VertexShaderID);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_path);
    char const * fragmentShader = readShader(fragment_path).c_str();
    glShaderSource(FragmentShaderID, 1, &fragmentShader , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

#endif