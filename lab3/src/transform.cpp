#include <transform.hpp>
#include <unordered_set>
// Helper function to apply a transformation matrix to a vertex

// Helper: Transform a vertex using a 4x4 matrix
glm::vec3 transformVertex(const glm::vec3& vertex, const glm::mat4& transform) {
    glm::vec4 temp = transform * glm::vec4(vertex, 1.0f);
    return glm::vec3(temp);
}

// Helper: Transform a normal (ignoring translation)
glm::vec3 transformNormal(const glm::vec3& normal, const glm::mat4& transform) {
    // Use the inverse-transpose of the rotation matrix for normals
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    return glm::normalize(normalMatrix * normal);
}

// Transform UVs (2D scaling + translation)
glm::vec2 transformUV(const glm::vec2& uv, float scale, const glm::vec2& offset) {
    return uv * scale + offset;
}

// Apply transformations to positions, normals, and UVs
void transformMeshAttributes(tinyobj::attrib_t& attrib, const tinyobj::mesh_t& mesh, 
                             const glm::mat4& transform, float uvScale, const glm::vec2& uvOffset) {
    std::vector<float>& vertices = attrib.vertices;
    std::vector<float>& normals = attrib.normals;
    std::vector<float>& texcoords = attrib.texcoords;
    std::unordered_set<int> transformed_vertices;
    std::unordered_set<int> transformed_normals;
    std::unordered_set<int> transformed_uvs;

    // Transform positions, normals, and UVs based on indices
    for (const auto& index : mesh.indices) {
        // Transform vertex positions
        int vertex_idx = 3 * index.vertex_index;
        if (transformed_vertices.count(vertex_idx) == 0) {
            glm::vec3 vertex(vertices[vertex_idx], 
                             vertices[vertex_idx + 1], 
                             vertices[vertex_idx + 2]);
            glm::vec3 transformed_vertex = transformVertex(vertex, transform);
            vertices[vertex_idx] = transformed_vertex.x;
            vertices[vertex_idx + 1] = transformed_vertex.y;
            vertices[vertex_idx + 2] = transformed_vertex.z;
            transformed_vertices.insert(vertex_idx);
        }

        // Transform normals (if they exist)
        if (index.normal_index >= 0) {
            int normal_idx = 3 * index.normal_index;
            if (transformed_normals.count(normal_idx) == 0) {
                glm::vec3 normal(normals[normal_idx], 
                                 normals[normal_idx + 1], 
                                 normals[normal_idx + 2]);
                glm::vec3 transformed_normal = transformNormal(normal, transform);
                normals[normal_idx] = transformed_normal.x;
                normals[normal_idx + 1] = transformed_normal.y;
                normals[normal_idx + 2] = transformed_normal.z;
                transformed_normals.insert(normal_idx);
            }
        }

        // Transform UVs (if they exist)
        if (index.texcoord_index >= 0) {
            int uv_idx = 2 * index.texcoord_index;
            if (transformed_uvs.count(uv_idx) == 0) {
                glm::vec2 uv(texcoords[uv_idx], texcoords[uv_idx + 1]);
                glm::vec2 transformed_uv = transformUV(uv, uvScale, uvOffset);
                texcoords[uv_idx] = transformed_uv.x;
                texcoords[uv_idx + 1] = transformed_uv.y;
                transformed_uvs.insert(uv_idx);
            }
        }
    }
}
