#ifndef OBJLOADER_H
#define OBJLOADER_H
#include "tiny_obj_loader.h" // Include the TinyOBJLoader header
#include <GL/glew.h>
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

#endif