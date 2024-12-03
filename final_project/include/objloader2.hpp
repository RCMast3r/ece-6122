#ifndef __OBJLOADER2_H__
#define __OBJLOADER2_H__

#include <chessComponent.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BoundingBox {
    aiVector3D min;
    aiVector3D max;

    BoundingBox() : min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}

    void Update(const aiVector3D& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }

    aiVector3D GetCenter() const {
        return (min + max) * 0.5f;
    }

    float GetScale() const {
        return std::max({max.x - min.x, max.y - min.y, max.z - min.z});
    }
};


void ProcessNode(aiNode* node, const aiScene* scene, std::vector<float>& vertices, std::vector<unsigned int>& indices);


void LoadModel(const std::string& path, std::vector<float>& vertices, std::vector<unsigned int>& indices);

// 
bool LoadOBJs(const char* path, std::vector<chessComponent>& gchessComponents);
#endif // __OBJLOADER2_H__