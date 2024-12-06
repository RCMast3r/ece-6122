#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>
#include <iostream>
#include "objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc


// Reads the multiple meshes OBJ file
bool loadAssImpLab3(const char* path, std::vector<chessComponent>& gchessComponents)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, (aiProcess_Triangulate | 
		                                            aiProcess_JoinIdenticalVertices | 
		                                            aiProcess_SortByPType));
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	
	// Node tree search starting point (Root)
	const aiNode* rNode = scene->mRootNode;

	// Loop assumes only one hierarchical node
	// Full hierarchical node trace is not added to the code
	// for this Lab
	for (unsigned int i = 0; i < rNode->mNumChildren; i++)
	{
		// Node point to the Children iteratively
		const aiNode * cNode = rNode->mChildren[i];

		// Read one by one the child sub-meshes
		// Loop only supports one mesh per Child
		const aiMesh* mesh = scene->mMeshes[cNode->mMeshes[0]];

		// Extract mesh properties
		meshPropsT meshProps = {mesh->HasBones(), mesh->HasFaces(), mesh->HasNormals(),
								mesh->HasPositions(), mesh->HasTangentsAndBitangents(),
								mesh->HasTextureCoords(0), mesh->HasVertexColors(0),
								mesh->GetNumUVChannels() };

		// Create a chess comonent object 

		// why heap allocate here ?
		chessComponent* gChessComponent = new chessComponent();

		// Store mesh properties
		gChessComponent->storeMeshProps(meshProps);
		
		// Grab child Node (mesh) name
		gChessComponent->storeComponentID(cNode->mName.C_Str());
		
		// Reserve buffers
		gChessComponent->reserveStorage(mesh->mNumVertices, mesh->mNumFaces);

		// Fill vertices positions
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D pos = mesh->mVertices[i];
			glm::vec3 mPos(pos.x, pos.y, pos.z);
			gChessComponent->addVertices(mPos);
		}

		std::cout << "Number of texture Coordinates " << mesh->GetNumUVChannels() << std::endl;
		// Fill vertices texture coordinates
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
			glm::vec3 mUVW(UVW.x, UVW.y, 0.f);
			gChessComponent->addTextureCor(mUVW);
		}

		// Fill vertices normals
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D n = mesh->mNormals[i];
			glm::vec3 mNorm(n.x, n.y, n.z);
			gChessComponent->addVerNormals(mNorm);
		}

		// Fill face indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			// Assume the model has only triangles.
			gChessComponent->addFaceIndices(&(mesh->mFaces[i].mIndices[0]));
		}

		// Access the material to get the Texture info
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString texturePath;
		// Check if the texture info is present
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
		{
			gChessComponent->storeTextureID(texturePath.C_Str());
		}

		// Push the class in the class vector
		gchessComponents.push_back(*gChessComponent);

		// Delete the Class to avoid mixing data 
		delete gChessComponent;
	}

	// The "scene" pointer will be deleted automatically by "importer"
	// delete gChessComponent;
	return true;
}

void processMesh(
	const aiMesh* mesh,
	std::vector<unsigned short>& indices,
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals
) {
	// Store initial size to adjust indices later
	unsigned short baseIndex = static_cast<unsigned short>(vertices.size());

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

		// Process texture coordinates
		if (mesh->mTextureCoords[0]) {
			aiVector3D uv = mesh->mTextureCoords[0][i];
			uvs.push_back(glm::vec2(uv.x, uv.y));
		} else {
			uvs.push_back(glm::vec2(0.0f, 0.0f)); // Default UV if none provided
		}

		// Process normals
		if (mesh->mNormals) {
			aiVector3D n = mesh->mNormals[i];
			normals.push_back(glm::vec3(n.x, n.y, n.z));
		}
	}

	// Process faces (indices)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(baseIndex + face.mIndices[j]);
		}
	}
}

// Recursive function to process nodes
void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes) {
    // Process each mesh in the node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		
        // Prepare a new mesh object
        Mesh newMesh;

        // Reserve space for vertex, normal, and texture data
        newMesh.vertices.reserve(mesh->mNumVertices);
        newMesh.uvs.reserve(mesh->mNumVertices);
        newMesh.normals.reserve(mesh->mNumVertices);
        newMesh.indices.reserve(mesh->mNumFaces * 3); // Assuming each face is a triangle

        glm::vec3 minCoord(FLT_MAX);
        glm::vec3 maxCoord(-FLT_MAX);

        // Extract vertices, normals, and UVs, and compute bounding box
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            glm::vec3 vertex(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

            // Update bounding box
            minCoord = glm::min(minCoord, vertex);
            maxCoord = glm::max(maxCoord, vertex);

            newMesh.vertices.push_back(vertex);
            newMesh.normals.push_back(glm::vec3(
                mesh->mNormals[v].x,
                mesh->mNormals[v].y,
                mesh->mNormals[v].z
            ));

            if (mesh->mTextureCoords[0]) {
                newMesh.uvs.push_back(glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                ));
            } else {
                newMesh.uvs.push_back(glm::vec2(0.0f, 0.0f));
            }
        }

        // Compute the center and scale
        glm::vec3 center = (minCoord + maxCoord) * 0.5f;
        float maxDimension = glm::max(maxCoord.x - minCoord.x, glm::max(maxCoord.y - minCoord.y, maxCoord.z - minCoord.z));

        // Center and normalize vertices
        for (auto& vertex : newMesh.vertices) {
            vertex = (vertex - center) / maxDimension;
        }

        // Extract indices (faces)
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int i = 0; i < face.mNumIndices; ++i) {
                newMesh.indices.push_back(face.mIndices[i]);
            }
        }

        // Generate buffers for the mesh
        glGenBuffers(1, &newMesh.vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, newMesh.vertices.size() * sizeof(glm::vec3), newMesh.vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &newMesh.uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, newMesh.uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, newMesh.uvs.size() * sizeof(glm::vec2), newMesh.uvs.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &newMesh.normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, newMesh.normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, newMesh.normals.size() * sizeof(glm::vec3), newMesh.normals.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &newMesh.elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh.indices.size() * sizeof(unsigned short), newMesh.indices.data(), GL_STATIC_DRAW);

		auto material = scene->mMaterials[mesh->mMaterialIndex];
		aiString texturePath;
		// Check if the texture info is present
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
		{
			// gChessComponent->storeTextureID(texturePath.C_Str());
			std::cout << texturePath.C_Str() <<std::endl;
			newMesh.textureFile = std::string(texturePath.C_Str());
		} else {
			std::cout <<"L no texture" <<std::endl;
		}
        // Add the mesh to the list
        meshes.push_back(newMesh);
    }

    // Process each child node recursively
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, meshes);
    }
}

bool loadobjfile(
    const char* path,
    std::vector<Mesh>& meshes  // This will store the meshes
) {
    Assimp::Importer importer;

    // Load the scene
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "%s\n", importer.GetErrorString());
        return false;
    }

    // Process the root node and its children to extract the meshes
    meshes.clear();  // Clear existing meshes if any
    processNode(scene->mRootNode, scene, meshes);

    // Return success
    return true;
}