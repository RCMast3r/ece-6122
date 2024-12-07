/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: object loader header file description. I really dont like opengl and textures / meshes have been the really annoying.

these functions have been hacked together from tutorials and provided examples. some of them work, most dont work the way they should.
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H
#include "tiny_obj_loader.h" // Include the TinyOBJLoader header
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// Include Chess Component Class    
#include "chessComponent.h"
#include "chessCommon.h"

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<tinyobj::material_t>& out_materials,
    std::vector<std::string>& texture_files, // Store texture paths
	std::vector<GLuint>& textureIDs, // Store texture IDs
    bool isLoadingBoard = false
);

struct Mesh {
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::string textureFile;
};

bool loadobjfile(
    const char* path,
    std::vector<Mesh>& meshes  // This will store the meshes
);


// Reads the multiple meshes OBJ file
bool loadAssImpLab3(
	const char* path,
	chessComponent& gchessComponents
);

#endif