// Include GLFW



#include <controls.hpp>


using namespace glm;


ControlsManager::ControlsState ControlsManager::computeMatricesFromInputs(OpenGLRuntimeParams params){

    ControlsState state;
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Calculate mouse movement
    // Clamp the vertical angle to prevent flipping
    _verticalAngle = glm::clamp(_verticalAngle, -glm::half_pi<float>(), glm::half_pi<float>());

    // Direction: Spherical to Cartesian conversion
    glm::vec3 direction(
        cos(_verticalAngle) * sin(_horizontalAngle), 
        sin(_verticalAngle),
        cos(_verticalAngle) * cos(_horizontalAngle)
    );

    // Update camera distance (initial distance)

    // Move the camera closer to the origin
    if (glfwGetKey(params.current_window, GLFW_KEY_W) == GLFW_PRESS) {
        _distanceFromCenter -= deltaTime * _speed; // Decrease distance
        if (_distanceFromCenter < 0.1f) _distanceFromCenter= 0.1f; // Prevent going too close to the origin
    }
    // Move the camera further from the origin
    if (glfwGetKey(params.current_window, GLFW_KEY_S) == GLFW_PRESS) {
        _distanceFromCenter += deltaTime * _speed; // Increase distance
    }

    // Update camera position based on direction vector and distance
    glm::vec3 position = direction * _distanceFromCenter; // Keep camera at specified distance from origin

    // Right vector
    glm::vec3 right = glm::cross(direction, glm::vec3(0, 1, 0));

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    // Move forward/backward (optional for finer control)
    if (glfwGetKey(params.current_window, GLFW_KEY_UP) == GLFW_PRESS) {
        _verticalAngle += deltaTime * 0.01;
    }
    if (glfwGetKey(params.current_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _verticalAngle -= deltaTime * 0.01;
    }
    if (glfwGetKey(params.current_window, GLFW_KEY_A) == GLFW_PRESS) {
        _horizontalAngle += deltaTime * 0.01f;
    }
    if (glfwGetKey(params.current_window, GLFW_KEY_D) == GLFW_PRESS) {
        _horizontalAngle -= deltaTime * 0.01f;
    }

    // Set up projection and view matrices
    state.projectionMatrix = glm::perspective(glm::radians(_initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
    state.viewMatrix = glm::lookAt(
        position,           // Camera is here
        glm::vec3(0, 0, 0), // and looks at the origin
        up                  // Head is up
    );

    if (_dbnceCnt <= 40)
	{
		_dbnceCnt++;
	}

    // The �L� key toggles the specular and diffuse components of the light on and off 
	// but leaves the ambient component unchanged.
	if (glfwGetKey(params.current_window, GLFW_KEY_L) == GLFW_PRESS) {
		if (_dbnceCnt >= 40)
		{ // Toggle light switch and restart the Debounce count
		    _toggleSDColor = !_toggleSDColor;
			_dbnceCnt = 0;
		}
	}
    state.lightSwitch = _toggleSDColor;

    return state;
}