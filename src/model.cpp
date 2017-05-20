#include "model.h"

const std::array<aiTextureType, 4> Model::mTextureTypes = {{
	aiTextureType_DIFFUSE,
    aiTextureType_SPECULAR,
    aiTextureType_HEIGHT,
	aiTextureType_AMBIENT
}};

Model::Model(VulkanState& vulkanState):
	mVulkanState(vulkanState),
	mVertexSize(sizeof(Vertex)),
	mPath(""),
	mFolder(""),
	imageInfoDiffuse(nullptr), imageInfoSpecular(nullptr), imageInfoHeight(nullptr), imageInfoAmbient(nullptr) 
{


}


Model::~Model() 
{	
}

void Model::init(std::string modelPath, unsigned int pFlags) 
{	
	init(modelPath.c_str(), pFlags);	
}

void Model::init(const char* modelPath, unsigned int pFlags)
{
	mPath = modelPath;
	mFolder = FileManager::getFilePath(std::string(modelPath));
	LOG("FOLDER:" << mFolder);
	Assimp::Importer importer;
	  // And have it read the given file with some example postprocessing
	  // Usually - if speed is not the most important aspect for you - you'll 
	  // propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(modelPath, pFlags);
	  
	  // If the import failed, report it
	if (!scene)
		throwError(importer.GetErrorString());	
	if (!scene->HasMeshes())
		throwError("No meshes found");

	processModel(*scene);
}



void Model::processModel(const aiScene& scene) 
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	for (size_t i = 0; i < scene.mNumMeshes; ++i) {

		aiMesh& mesh = *scene.mMeshes[i];
		Mesh meshInfo;

		bool hasPositions = mesh.HasPositions();
		bool hasNormals = mesh.HasNormals();
		bool hasTangentsAndBitangents = mesh.HasTangentsAndBitangents();
		bool hasTexCoords = mesh.HasTextureCoords(0);

		// Vertices
		meshInfo.baseVertex = vertices.size();
		for (size_t j = 0; j < mesh.mNumVertices; ++j) {
			Vertex vertex;
			if (hasPositions) 
				convertVector(mesh.mVertices[j], vertex.pos);
			if (hasNormals) 
				convertVector(mesh.mNormals[j], vertex.normal);
			if (hasTangentsAndBitangents) {
				convertVector(mesh.mTangents[j], vertex.tangent);
				convertVector(mesh.mBitangents[j], vertex.bitangent);
			}
			if (hasTexCoords) 
				convertVector(mesh.mTextureCoords[0][j], vertex.texCoord);
			vertices.push_back(vertex);
		}
		meshInfo.numVertices = vertices.size() - meshInfo.numVertices;

		// Indices
		meshInfo.baseIndex = indices.size();
		for (size_t j = 0; j < mesh.mNumFaces; ++j) 
			for (size_t k = 0; k < mesh.mFaces[j].mNumIndices; ++k)
				indices.push_back(mesh.mFaces[j].mIndices[k]);	
		meshInfo.numIndices = indices.size() - meshInfo.baseIndex;

		
		// Textures
		meshInfo.materialIndex = mesh.mMaterialIndex;
		aiMaterial& material = *scene.mMaterials[mesh.mMaterialIndex];		
		auto it = mMaterialIndexToMaterial.find(mesh.mMaterialIndex);
		if (it == mMaterialIndexToMaterial.end()) {
			Material materialInfo;
			for (size_t j = 0; j < mTextureTypes.size(); ++j) {
				aiTextureType textureType = mTextureTypes[j];
				size_t numMaterials = material.GetTextureCount(textureType); 
				for (size_t k = 0; k < numMaterials; ++k) {
					aiString texturePath;
				
					material.GetTexture(textureType, k, &texturePath);	
					std::string fullTexturePath = mFolder + "/";
					fullTexturePath += texturePath.C_Str();

					TextureDesc textureDesc(fullTexturePath);
					//LOG("BEFORE LOAD:" << textureDesc.filename);
					ImageInfo* imageInfo = TextureManager::load(
							mVulkanState, 
							mVulkanState.commandPool, 
							mVulkanState.graphicsQueue, 
							textureDesc);


					LOG("AFTER LOAD");
					switch(textureType) {
						case aiTextureType_DIFFUSE:
							materialInfo.diffuseImages.push_back(imageInfo);	
							break;
						case aiTextureType_SPECULAR:
							materialInfo.specularImages.push_back(imageInfo);	
							break;
						case aiTextureType_HEIGHT:
							materialInfo.heightImages.push_back(imageInfo);	
							break;
						case aiTextureType_AMBIENT:
							materialInfo.ambientImages.push_back(imageInfo);
							break;
						default:
							break;
					}	
				}
			}
			mMaterialIndexToMaterial[mesh.mMaterialIndex] = materialInfo;
		}
	}
}

void Model::convertVector(const aiVector3D& src, glm::vec3& dest)
{
    dest.x = src.x;
    dest.y = src.y; 
    dest.z = src.z;
}

void Model::convertVector(const aiVector3D& src, glm::vec2& dest)
{
    dest.x = src.x;
    dest.y = src.y; 
}


void Model::throwError(const char* error) 
{
	std::string errorStr = error;
	throwError(errorStr);
}

void Model::throwError(std::string& error) 
{
	error += " for scene: " + mPath;
	throw std::runtime_error(error);
}
