#include "texture_manager.h"

TextureManager& TextureManager::getInstance() 
{
	static TextureManager textureManager;
	return textureManager;
}

ImageInfo* TextureManager::load(
			VulkanState& state, 
			const VkCommandPool& cmdPool, 
			const VkQueue& cmdQueue,
			const TextureDesc& textureDesc)

{
	TextureManager& tm = getInstance();
	std::lock_guard<std::mutex> guard(tm.lock);
	// Check map, load image thread safely
	TextureData textureData;
	textureData.load(textureDesc.filename.c_str(), textureDesc.reqComp);

	auto it = tm.mPool.find(textureDesc);
	if (it != tm.mPool.end())
		return it->second;	

	ImageInfo* info = new ImageInfo(state.device, textureData.width, textureData.height);
	ImageHelper::createStagedImage(*info, textureData, state, cmdPool, cmdQueue);
	tm.mPool[textureDesc] = info;
	return info;
}

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
/*	
	for (auto it = mPool.begin(); it != mPool.end(); ++it) 
        if (it->second)
			delete it->second;
    mPool.clear();
*/
}




