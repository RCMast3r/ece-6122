// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <controls.hpp>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::vec3 position = glm::vec3( 0, 0, 2 ); 
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
float initialFoV = 100.0f;
float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
float distanceFromCenter = 2.0f; // Set initial distance from the origin

glm::vec3 getDirectionToCenter() {
    // Calculate the direction vector from the camera position to the center
    glm::vec3 directionToCenter = glm::vec3(0, 0, 0) - position; // Center is at (0, 0, 0)
    // Normalize the direction vector
    return glm::normalize(directionToCenter);
}

void computeMatricesFromInputs(){

    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Calculate mouse movement
    // Clamp the vertical angle to prevent flipping
    verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>(), glm::half_pi<float>());

    // Direction: Spherical to Cartesian conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle), 
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Update camera distance (initial distance)

    // Move the camera closer to the origin
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        distanceFromCenter -= deltaTime * speed; // Decrease distance
        if (distanceFromCenter < 0.1f) distanceFromCenter= 0.1f; // Prevent going too close to the origin
    }
    // Move the camera further from the origin
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        distanceFromCenter += deltaTime * speed; // Increase distance
    }

    // Update camera position based on direction vector and distance
    position = direction * distanceFromCenter; // Keep camera at specified distance from origin

    // Right vector
    glm::vec3 right = glm::cross(direction, glm::vec3(0, 1, 0));

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    // Move forward/backward (optional for finer control)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        verticalAngle += deltaTime * 0.8;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        verticalAngle -= deltaTime * 0.8;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngle += deltaTime * 0.8f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngle -= deltaTime * 0.8f;
    }

    // Set up projection and view matrices
    ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
    ViewMatrix = glm::lookAt(
        position,           // Camera is here
        glm::vec3(0, 0, 0), // and looks at the origin
        up                  // Head is up
    );

    lastTime = currentTime;	
}