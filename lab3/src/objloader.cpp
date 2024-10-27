#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <iostream>
#include <glm/glm.hpp>

#include <objloader.hpp>

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals
) {
    printf("Loading OBJ file %s...\n", path);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    // Load the OBJ file
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path);
    if (!err.empty()) {
        std::cerr << err << std::endl; // Print the error if it exists
    }
    if (!ret) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return false;
    }

    // Reserve space for the output vectors
    out_vertices.reserve(attrib.vertices.size() / 3);
    out_uvs.reserve(attrib.texcoords.size() / 2);
    out_normals.reserve(attrib.normals.size() / 3);

    // Populate the output vectors
    for (size_t i = 0; i < shapes.size(); i++) {
        size_t index_offset = 0;
        for (size_t j = 0; j < shapes[i].mesh.num_face_vertices.size(); j++) {
            int fnum = shapes[i].mesh.num_face_vertices[j]; // Number of vertices in a face
            for (size_t v = 0; v < fnum; v++) {
                tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];

                // Push vertex position
                out_vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );

                // Push texture coordinates (if they exist)
                if (idx.texcoord_index >= 0) {
                    out_uvs.emplace_back(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                } else {
                    out_uvs.emplace_back(0.0f, 0.0f); // Default UV if not available
                }

                // Push vertex normals (if they exist)
                if (idx.normal_index >= 0) {
                    out_normals.emplace_back(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                } else {
                    out_normals.emplace_back(0.0f, 0.0f, 0.0f); // Default normal if not available
                }
            }
            index_offset += fnum; // Move to the next face
        }
    }
    // Normalize vertices
    if (!out_vertices.empty()) {
        glm::vec3 minBounds = out_vertices[0];
        glm::vec3 maxBounds = out_vertices[0];

        // Find bounding box
        for (const auto& vertex : out_vertices) {
            minBounds = glm::min(minBounds, vertex);
            maxBounds = glm::max(maxBounds, vertex);
        }

        // Compute the center and size of the bounding box
        glm::vec3 size = maxBounds - minBounds;
        glm::vec3 center = (maxBounds + minBounds) * 0.5f;

        // Normalize vertices to fit in the range [-1, 1]
        for (auto& vertex : out_vertices) {
            vertex = (vertex - center) / glm::max(size.x, glm::max(size.y, size.z)); // Scale to fit
        }
    }

    return true;
}