#include "texture_manager.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TextureData::TextureData():
   mWidth(0), mHeight(0), mChannels(0), mSize(0), mPixels(nullptr)
{

}

TextureData::~TextureData()
{
	if (mPixels)
		stbi_image_free(mPixels);
}

stbi_uc* TextureData::load(const char* resource, int reqComp)
{
	std::string filenameStr = FileManager::getInstance().getResourcePath(resource);
	const char* filename = filenameStr.c_str();
	mPixels = stbi_load(filename, &mWidth, &mHeight, &mChannels, reqComp); 
	mSize = mWidth * mHeight * reqComp;
	if (!mPixels || !mSize) {
		char err[256];
		sprintf(err, "TEXTURE ERROR reason: %s path:\"%s\" w:%d h:%d channels:%d reqComp:%d ", stbi_failure_reason(), filename, mWidth, mHeight, mChannels, reqComp); 
		throw std::runtime_error(err);
	}
	
	LOG("TEXTURE LOADED path:\""<< filename << "\" w:" << mWidth << " h:" << mHeight << " channels:" << mChannels << " reqComp:"<< reqComp << " size:" << mSize);
	return mPixels;
}

int TextureData::getWidth() const 
{
	return mWidth;
}

int TextureData::getHeight() const 
{
	return mHeight;
}

int TextureData::getChannels() const
{
	return mChannels;
}

stbi_uc* TextureData::getPixels()
{
	return mPixels;
}

uint64_t TextureData::getSize() const 
{
	return mSize;
}


TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()

{

}
