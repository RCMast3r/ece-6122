#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 transformVertex(const glm::vec3& vertex, const glm::mat4& transform);
glm::vec3 transformNormal(const glm::vec3& normal, const glm::mat4& transform);
glm::vec2 transformUV(const glm::vec2& uv, float scale, const glm::vec2& offset);

void transformMeshAttributes(tinyobj::attrib_t& attrib, const tinyobj::mesh_t& mesh, const glm::mat4& transform, float uvScale, const glm::vec2& uvOffset);
#endif // __TRANSFORM_H__