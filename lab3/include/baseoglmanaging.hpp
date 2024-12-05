#ifndef __BASEOGLMANAGING_H__
#define __BASEOGLMANAGING_H__

// #include 
#include <optional>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

struct OpenGLRuntimeParams
{
    GLFWwindow* current_window;
    GLuint programID;
    GLuint matrixID;  
    GLuint viewMatrixID;
    GLuint modelMatrixID;
    GLuint textureID;
    GLuint lightSwitchID;
    GLuint lightID;
};

std::optional<OpenGLRuntimeParams> setupScene();

#endif // __BASEOGLMANAGING_H__