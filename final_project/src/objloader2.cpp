#include <objloader2.hpp>


bool handleComponentCreation(const aiScene* scene, const aiNode *cNode, aiMesh* mesh, std::vector<chessComponent>& gchessComponents)
{
    meshPropsT meshProps = {mesh->HasBones(), mesh->HasFaces(), mesh->HasNormals(),
								mesh->HasPositions(), mesh->HasTangentsAndBitangents(),
								mesh->HasTextureCoords(0), mesh->HasVertexColors(0),
								mesh->GetNumUVChannels() };
    chessComponent gChessComponent;
    gChessComponent.storeMeshProps(meshProps);
    gChessComponent.storeComponentID(cNode->mName.C_Str());
    gChessComponent.reserveStorage(mesh->mNumVertices, mesh->mNumFaces);
    std::cout << "Number of texture Coordinates obj2 " << mesh->GetNumUVChannels() << std::endl;
    std::cout << "Number of verts obj2 " << mesh->mNumVertices << std::endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D pos = mesh->mVertices[i];
        glm::vec3 mPos(pos.x, pos.y, pos.z);
        gChessComponent.addVertices(mPos);
        // aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        // glm::vec3 mUVW(UVW.x, UVW.y, 0.f);
        // gChessComponent.addTextureCor(mUVW);
        aiVector3D n = mesh->mNormals[i];
        glm::vec3 mNorm(n.x, n.y, n.z);
        gChessComponent.addVerNormals(mNorm);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        // Assume the model has only triangles.
        gChessComponent.addFaceIndices(&(mesh->mFaces[i].mIndices[0]));
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString texturePath;
    // Check if the texture info is present
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
    {
        gChessComponent.storeTextureID(texturePath.C_Str());
    } else {
        std::cout <<"no texture for" <<std::endl;
    }
    gchessComponents.push_back(gChessComponent);
    return true;
}

void ProcessMesh(aiMesh* mesh, std::vector<float>& normalizedVertices, std::vector<unsigned int>& indices) {
    BoundingBox bbox;

    // Collect vertices and update bounding box
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D vertex = mesh->mVertices[i];
        bbox.Update(vertex);
        normalizedVertices.push_back(vertex.x);
        normalizedVertices.push_back(vertex.y);
        normalizedVertices.push_back(vertex.z);
    }

    // Collect indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Normalize vertices by shifting to the center and scaling
    aiVector3D center = bbox.GetCenter();
    float scale = bbox.GetScale();
    for (unsigned int i = 0; i < normalizedVertices.size() / 3; i++) {
        normalizedVertices[i * 3 + 0] = (normalizedVertices[i * 3 + 0] - center.x) / scale;
        normalizedVertices[i * 3 + 1] = (normalizedVertices[i * 3 + 1] - center.y) / scale;
        normalizedVertices[i * 3 + 2] = (normalizedVertices[i * 3 + 2] - center.z) / scale;
    }
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<chessComponent>& gchessComponents) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        handleComponentCreation(scene, node, mesh, gchessComponents);
        ProcessMesh(mesh, vertices, indices);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, vertices, indices, gchessComponents);
    }
}

// Load the model
void LoadModel(const std::string& path, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<chessComponent>& gchessComponents) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene, vertices, indices, gchessComponents);
}

// making this function just for the normal .obj chess piece files. this is ass and i hate it.

// why cant anything work, opengl is very garbage to work with. im going to have to GLEW 
// my sanity back together if I have to look at any more of this ancient malformed code.
bool LoadOBJs(const char* path, std::vector<chessComponent>& gchessComponents)
{

    std::vector<float> verts;
    std::vector<unsigned int> inds;
    LoadModel(path, verts, inds, gchessComponents);
    return true;
}