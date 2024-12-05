#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <baseoglmanaging.hpp>

#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class ControlsManager
{
    public:
        struct ControlsState
        {
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            bool lightSwitch;
        };
        ControlsManager() {}
        ControlsState computeMatricesFromInputs(OpenGLRuntimeParams params);
    
    private:
        float _horizontalAngle = 3.14f;
        float _verticalAngle = 0.0f;
        float _initialFoV = 100.0f;
        float _speed = 0.03f;
        float _mouseSpeed = 0.005f;
        float _distanceFromCenter = 2.0f;
        // glm::vec3 _position = glm::vec3( 0, 0, 2 );
        bool _toggleSDColor = true;
        int _dbnceCnt = 20;
        
};

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif