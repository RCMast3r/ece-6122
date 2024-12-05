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
using namespace glm;


#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
// #include <tiny_obj_loader.h>

void renderObject(const std::vector<glm::vec3>& vertices, 
                  const std::vector<glm::vec2>& uvs, 
                  const std::vector<glm::vec3>& normals, 
                  GLuint textureID) {
    // Bind the object's texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Enable vertex attributes and bind buffers (pseudo-code, adapt to your buffer setup)
    glEnableVertexAttribArray(0); // Position
    glEnableVertexAttribArray(1); // UV
    glEnableVertexAttribArray(2); // Normal

    // Upload data (assuming VAO and VBOs are already bound)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, &uvs[0]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, &normals[0]);

    // Draw the object
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

int main( void )
{
	auto params = setupScene();
	if(!params)
	{
		std::cout << "error occured during setup, exiting" <<std::endl;
		return 0;
	}
	// Initialize GLFW
	// if( !glfwInit() )
	// {
	// 	fprintf( stderr, "Failed to initialize GLFW\n" );
	// 	getchar();
	// 	return -1;
	// }

	// glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// // Open a window and create its OpenGL context
	// window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	// if( window == NULL ){
	// 	fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
	// 	getchar();
	// 	glfwTerminate();
	// 	return -1;
	// }
	// glfwMakeContextCurrent(window);

	// // Initialize GLEW
	// glewExperimental = true; // Needed for core profile
	// if (glewInit() != GLEW_OK) {
	// 	fprintf(stderr, "Failed to initialize GLEW\n");
	// 	getchar();
	// 	glfwTerminate();
	// 	return -1;
	// }

	// // Ensure we can capture the escape key being pressed below
	// glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // // Hide the mouse and enable unlimited movement
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // // Set the mouse at the center of the screen
    // glfwPollEvents();
    // glfwSetCursorPos(window, 1024/2, 768/2);

	// // Dark blue background
	// glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// // Enable depth test
	// glEnable(GL_DEPTH_TEST);
	// // Accept fragment if it is closer to the camera than the former one
	// glDepthFunc(GL_LESS); 

	// // Cull triangles which normal is not towards the camera
	// glEnable(GL_CULL_FACE);

	// GLuint VertexArrayID;
	// glGenVertexArrays(1, &VertexArrayID);
	// glBindVertexArray(VertexArrayID);

	// // Create and compile our GLSL program from the shaders
	// GLuint programID = LoadShaders( "data/StandardShading.vertexshader", "data/StandardShading.fragmentshader" );

	// // Get a handle for our "MVP" uniform
	// GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	// GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	// GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	// GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	// Read our .obj file
	std::vector<glm::vec3> vertices, vertices_board;
	std::vector<glm::vec2> uvs, uvs_board;
	std::vector<glm::vec3> normals, normals_board; 
	std::vector<tinyobj::material_t> mats, mats_board;
	std::vector<std::string> texture_files, texture_files_board;
	std::vector<GLuint> textureids, textureids_board;

	bool res = loadOBJ("data/chess3.obj", vertices, uvs, normals, mats, texture_files, textureids);
	bool re2 = loadOBJ("data/12951_Stone_Chess_Board_v1_L3.obj", vertices_board, uvs_board, normals_board, mats_board, texture_files_board, textureids_board, true);

	std::vector<GLuint> textures;
    for (const auto& tex_file : texture_files) {
        textures.push_back(loadBMP_custom(tex_file.c_str()));  
    }
	for (const auto& tex_file : texture_files_board) {
        textures.push_back(loadBMP_custom(tex_file.c_str()));  
    }

	vertices.insert(vertices.end(), vertices_board.begin(), vertices_board.end() );

	// uvs.insert(uvs.end(), uvs_board.begin(), uvs_board.end());
	// normals.insert(normals.end(), normals_board.begin(), normals_board.end());
	// Assuming each material corresponds to a different object, calculate the offsets
    size_t totalVertices = vertices.size();
    size_t objectCount = textures.size();  // Assume one object per texture


// setuo
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	// glUseProgram(params.programID);
	// GLuint LightID = glGetUniformLocation(params.programID, "LightPosition_worldspace");
	auto p = *p;
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(p.programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset/glUniform1i
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		for (size_t i = 0; i < textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
			glUniform1i(TextureID, 0);
            // glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			// glUniform1i(textures[i], 0);
        }

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	for (auto tex : textures) glDeleteTextures(1, &tex);
	glfwTerminate();
	return 0;
}


