#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <iostream>
#include <glm/glm.hpp>

#include <objloader.hpp>
#include <transform.hpp>
#include <map>
#include <texture.hpp>

std::map<std::string, GLuint> textureCache;

GLuint getTextureID(const std::string& texturePath) {
    // Check if the texture is already loaded
    if (textureCache.find(texturePath) != textureCache.end()) {
        return textureCache[texturePath];
    }

    // Load the texture
    GLuint textureID;
    if (texturePath.find(".bmp") != std::string::npos) {
        textureID = loadBMP_custom((std::string("data/")+ texturePath).c_str());
    } else if (texturePath.find(".dds") != std::string::npos) {
        textureID = loadDDS(texturePath.c_str());
    } else {
        std::cerr << "Unsupported texture format: " << texturePath << std::endl;
        return 0;
    }

    textureCache[texturePath] = textureID; // Cache it
    return textureID;
}


bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<tinyobj::material_t>& out_materials,
    std::vector<std::string>& texture_files, // Store texture paths
    std::vector<GLuint>& textureIDs,
    bool isLoadingBoard
) {
    printf("Loading OBJ file %s...\n", path);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err, warn;

    // Load the OBJ file along with its materials
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path);

    if (!warn.empty()) std::cout << "Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Error: " << err << std::endl;
    if (!ret) return false;

    // Reserve space for output vectors
    out_vertices.reserve(attrib.vertices.size() / 3);
    out_uvs.reserve(attrib.texcoords.size() / 2);
    out_normals.reserve(attrib.normals.size() / 3);
    out_materials = materials; // Store loaded materials

    // Example: Create a transformation matrix (scaling + rotation + translation)
    if(!isLoadingBoard)
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0, 1, 0));
        transform = glm::translate(transform, glm::vec3(1.0f, 0.0f, 0.0f));
        transformMeshAttributes(attrib, shapes[0].mesh, transform, 1, {1.0, 1.0});
    } else {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(270.0f), glm::vec3(1, 0, 0));
        transform = glm::translate(transform, glm::vec3(10000.0f, 3000.0f, 4000.0f));
        transformMeshAttributes(attrib, shapes[0].mesh, transform, 1, {1.0, 1.0});       
    }

    // Iterate over shapes and extract vertex data
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t j = 0; j < shape.mesh.num_face_vertices.size(); j++) {
            int fnum = shape.mesh.num_face_vertices[j]; // Number of vertices per face
            for (size_t v = 0; v < fnum; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // Vertex position
                out_vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );

                // Texture coordinates (default to 0 if missing)
                if (idx.texcoord_index >= 0) {
                    out_uvs.emplace_back(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                } else {
                    out_uvs.emplace_back(0.0f, 0.0f);
                }

                // Vertex normals (default to 0 if missing)
                if (idx.normal_index >= 0) {
                    out_normals.emplace_back(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                } else {
                    out_normals.emplace_back(0.0f, 0.0f, 0.0f);
                }
            }
            index_offset += fnum; // Move to the next face
        }
    }

    // Collect texture file paths from materials
    for (const auto& material : materials) {
        if (!material.diffuse_texname.empty()) {
            std::string texture_path = "data/"+material.diffuse_texname;
            texture_files.push_back(texture_path); // Store texture path
            std::cout << "Loaded texture: " << texture_path << std::endl;
        }
    }

    // Normalize vertices to fit within [-1, 1]
    if(!isLoadingBoard)
    {
        if (!out_vertices.empty()) {
            glm::vec3 minBounds = out_vertices[0];
            glm::vec3 maxBounds = out_vertices[0];

            // Compute bounding box
            for (const auto& vertex : out_vertices) {
                minBounds = glm::min(minBounds, vertex);
                maxBounds = glm::max(maxBounds, vertex);
            }

            glm::vec3 size = maxBounds - minBounds;
            glm::vec3 center = (maxBounds + minBounds) * 0.5f;
            // Normalize each vertex
            for (auto& vertex : out_vertices) {
                vertex = (vertex - center) / glm::max(size.x, glm::max(size.y, size.z));
            }
        }
    } else {
       if (!out_vertices.empty()) {
            glm::vec3 minBounds = out_vertices[0];
            glm::vec3 maxBounds = out_vertices[0];

            // Compute bounding box
            for (const auto& vertex : out_vertices) {
                minBounds = glm::min(minBounds, vertex);
                maxBounds = glm::max(maxBounds, vertex);
            }

            glm::vec3 size = maxBounds - minBounds;
            glm::vec3 center = (maxBounds + minBounds) * 0.5f;
            center.y += 7.0;
            // Normalize each vertex
            for (auto& vertex : out_vertices) {
                vertex = (vertex - center) / glm::max(size.x, glm::max(size.y, size.z));
            }
        } 
    }
    
    for (const auto& material : out_materials) {
        if (!material.diffuse_texname.empty()) {
            GLuint textureID = getTextureID(material.diffuse_texname);
            if (textureID != 0) {
                textureIDs.push_back(textureID); // Store the texture ID
                std::cout << "Loaded texture: " << material.diffuse_texname << std::endl;
            }
        }
    }
    std::cout << "Vertices: " << out_vertices.size() << std::endl;
    std::cout << "UVs: " << out_uvs.size() << std::endl;
    std::cout << "Normals: " << out_normals.size() << std::endl;
    std::cout << "Materials: " << out_materials.size() << std::endl;
    std::cout << "Textures: " << texture_files.size() << std::endl;

    return true;
}