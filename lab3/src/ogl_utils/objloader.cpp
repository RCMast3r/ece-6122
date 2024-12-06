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


bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	}

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

	fclose(file);
	return true;
}


#ifdef USE_ASSIMP // don't use this #define, it's only for me (it AssImp fails to compile on your machine, at least all the other tutorials still work)

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
){

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
	if( !scene) {
		fprintf( stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

	// Fill vertices positions
	vertices.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	uvs.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		uvs.push_back(glm::vec2(UVW.x, UVW.y));
	}

	// Fill vertices normals
	normals.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D n = mesh->mNormals[i];
		normals.push_back(glm::vec3(n.x, n.y, n.z));
	}


	// Fill face indices
	indices.reserve(3*mesh->mNumFaces);
	for (unsigned int i=0; i<mesh->mNumFaces; i++){
		// Assume the model has only triangles.
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	
	// The "scene" pointer will be deleted automatically by "importer"
	return true;
}

#endif


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