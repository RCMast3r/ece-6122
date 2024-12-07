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


/**
 * @brief Loads an OBJ file and extracts its geometry, materials, and texture information.
 * 
 * This function loads an OBJ file and processes its vertices, UVs, normals, materials,
 * and texture data. It stores the extracted data in the provided output vectors.
 * It also manages texture file paths and texture IDs for later use with OpenGL.
 *
 * @param path The file path to the OBJ file to load.
 * @param out_vertices A reference to a vector where the vertex positions (glm::vec3) will be stored.
 * @param out_uvs A reference to a vector where the texture coordinates (glm::vec2) will be stored.
 * @param out_normals A reference to a vector where the vertex normals (glm::vec3) will be stored.
 * @param out_materials A reference to a vector where materials from the OBJ file will be stored.
 * @param texture_files A reference to a vector where the paths to texture files will be stored.
 * @param textureIDs A reference to a vector where OpenGL texture IDs (GLuint) will be stored after loading textures.
 * @param isLoadingBoard A flag (default false) indicating if the function is loading a chessboard (optional).
 * 
 * @return true if the OBJ file was loaded successfully, false otherwise.
 */
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


/**
 * @brief Represents a 3D mesh with vertex data, textures, and OpenGL buffers.
 * 
 * This struct contains data for a 3D mesh, including vertex positions, texture coordinates,
 * normals, indices for element drawing, and the texture file associated with the mesh.
 * It also includes OpenGL buffer IDs to store the data efficiently for rendering.
 */
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


/**
 * @brief Loads an OBJ file and extracts its meshes. is used for the chess pieces as they are special and need some hand holding
 * 
 * This function loads an OBJ file and processes its contents to extract all the meshes,
 * including their vertex data, UVs, normals, and texture information. The meshes are stored
 * in the provided vector of Mesh objects.
 *
 * @param path The file path to the OBJ file to load.
 * @param meshes A reference to a vector where the extracted meshes will be stored.
 * 
 * @return true if the OBJ file was successfully loaded and meshes were extracted, false otherwise.
 */
bool loadobjfile(
    const char* path,
    std::vector<Mesh>& meshes  // This will store the meshes
);



// Reads the multiple meshes OBJ file
bool loadAssImpLab3(
	const char* path,
	std::vector<chessComponent>& gchessComponents
);

#endif