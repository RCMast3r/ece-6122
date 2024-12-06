// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );


// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;



void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	// glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	// glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	// horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	// verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void applyOffsetToMesh(Mesh& mesh, float offsetX) {
    for (auto& vertex : mesh.vertices) {
        vertex.x += offsetX;
    }

    // Update the OpenGL buffer with the modified vertices
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer for safety
}

GLuint createDummyTexture() {
    GLuint dummyTexture;
    glGenTextures(1, &dummyTexture);
    glBindTexture(GL_TEXTURE_2D, dummyTexture);

    // Create a single-color 2x2 texture (e.g., white)
    unsigned char whitePixel[4] = {255, 255, 255, 255}; // RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return dummyTexture;
}

GLuint createDummyTexture2() {
    GLuint dummyTexture;
    glGenTextures(1, &dummyTexture);
    glBindTexture(GL_TEXTURE_2D, dummyTexture);

    // Create a single-color 2x2 texture (e.g., white)
    unsigned char whitePixel[4] = {122, 0, 0, 255}; // RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return dummyTexture;
}

int main(void) {
    // (Initialization code remains unchanged)
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "Dummy Texture Example", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Setup OpenGL settings

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("data/StandardShading.vertexshader", "data/StandardShading.fragmentshader");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    GLuint dummyTexture = createDummyTexture(); // Use dummy texture
    GLuint dummyTextur2 = createDummyTexture2(); // Use dummy texture
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    GLuint NormalTexture = loadBMP_custom("data/wooddark0.bmp");
    // (Load your OBJ file as before)
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    glm::vec3 centroid;

    std::vector<Mesh> meshes;
    loadobjfile("data/chess-mod.obj", meshes);
    for (size_t i = 0; i < meshes.size(); ++i) {
        applyOffsetToMesh(meshes[i], static_cast<float>(i));
    }
    
    
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(4, 4, 4);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


        

        for (size_t i = 0; i < meshes.size(); ++i) {
            // Bind buffers for the current mesh
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].uvbuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].normalbuffer);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i].elementbuffer);
            glDrawElements(GL_TRIANGLES, meshes[i].indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, NormalTexture);
            // // Set our "NormalTextureSampler" sampler to use Texture Unit 1
            // glUniform1i(TextureID, 0);

            if(i < 5)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, dummyTexture);
                glUniform1i(TextureID, 0);
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, dummyTextur2);
                glUniform1i(TextureID, 0);
            }
            

            // Disable vertex arrays after drawing
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // glDeleteBuffers(1, &vertexbuffer);
    // glDeleteBuffers(1, &uvbuffer);
    // glDeleteBuffers(1, &normalbuffer);
    // glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &dummyTexture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
    return 0;
}

