#ifndef __OPENGLINIT_H__
#define __OPENGLINIT_H__


#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
// functions that handle the opengl window creation / opengl initialization


/// @brief handles:
//      1. window hint setup
//      2. window creation w/ specific dimensions and title
//      3. initializes the glfw library 
// returns window pointer if successful, std::nullopt otherwise
std::optional<GLFWwindow*> setupOpenGLWindow();
#endif // __OPENGLINIT_H__