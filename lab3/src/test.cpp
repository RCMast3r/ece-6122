// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
// Include GLEW

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;


#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
// #include <tiny_obj_loader.h>
#include <baseoglmanaging.hpp>
#include <iostream>

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
	ControlsManager controls;
	auto params = setupScene();
	GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
	if(!params)
	{
		std::cout << "error occured during setup, exiting" <<std::endl;
		return 0;
	}

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

	uvs.insert(uvs.end(), uvs_board.begin(), uvs_board.end());
	normals.insert(normals.end(), normals_board.begin(), normals_board.end());
	// Assuming each material corresponds to a different object, calculate the offsets
    size_t totalVertices = vertices.size();
    size_t objectCount = textures.size(); 

	// 

// setup buffers for 
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
	auto p = *params;
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(p.programID);

		// Compute the MVP matrix from keyboard and mouse input
		auto cntrls = controls.computeMatricesFromInputs(p);

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = cntrls.projectionMatrix * cntrls.viewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(p.matrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(p.modelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(p.viewMatrixID, 1, GL_FALSE, &cntrls.viewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(p.lightID, lightPos.x, lightPos.y, lightPos.z);

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
		// for (size_t i = 0; i < textures.size(); ++i) {
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, textures[i]);
		// 	glUniform1i(p.textureID, 0);
        //     // glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		// 	// glUniform1i(textures[i], 0);
        // }

		for (const auto texture: textures)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			// Set our "myTextureSampler" sampler to use Texture Unit 0
			glUniform1i(p.textureID, 0);
		}
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(p.current_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(p.current_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(p.current_window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(p.programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	for (auto tex : textures) glDeleteTextures(1, &tex);
	glfwTerminate();
	return 0;
}


