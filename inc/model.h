#ifndef AMVK_MODEL_H
#define AMVK_MODEL_H

#include "macro.h"

#include <vulkan/vulkan.h>
#include <cstring>
#include <cstddef>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "buffer_helper.h"
#include "vulkan_image_creator.h"
#include "vulkan_image_info.h"
#include "vulkan_render_pass_creator.h"
#include "vulkan_state.h"
#include "texture_manager.h"
#include "pipeline_creator.h"
#include "timer.h"
#include "camera.h"

class Model {
public:
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texCoord;
	};

	struct UBO {
	    glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct Material {
		std::vector<ImageInfo*> 
			diffuseImages, 
			specularImages, 
			heightImages, 
			ambientImages; 
	}; 

	struct Mesh {
		uint32_t baseIndex, numIndices;
		uint32_t baseVertex, numVertices;
		uint32_t materialIndex;
	};

	Model(VulkanState& vulkanState);
	~Model();

	void init(const char* modelPath, 
		unsigned int pFlags = 
		aiProcess_CalcTangentSpace
		| aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices  
		| aiProcess_SortByPType);
	
	void init(std::string modelPath, 
		unsigned int pFlags = 
		aiProcess_CalcTangentSpace
		| aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices  
		| aiProcess_SortByPType); 

	void processModel(const aiScene& scene);
private:
	static void convertVector(const aiVector3D& src, glm::vec3& dest);
	static void convertVector(const aiVector3D& src, glm::vec2& dest);
	static const std::array<aiTextureType, 4> mTextureTypes;
	
	void throwError(const char* error);
	void throwError(std::string& error);
	
	VulkanState mVulkanState;
	uint32_t mVertexSize;

	std::string mPath;
	ImageInfo *imageInfoDiffuse, *imageInfoSpecular, *imageInfoHeight, *imageInfoAmbient; 
	std::unordered_map<uint32_t, Material> mMaterialIndexToMaterial;	
};

#endif
