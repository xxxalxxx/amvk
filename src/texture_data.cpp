#include "texture_data.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TextureData::TextureData():
   width(0), height(0), channels(0), size(0), pixels(nullptr)
{

}

TextureData::~TextureData()
{
	if (pixels)
		stbi_image_free(pixels);
}

stbi_uc* TextureData::load(const char* resource, int reqComp)
{
	std::string filenameStr(resource);
	const char* filename = filenameStr.c_str();
	pixels = stbi_load(filename, &width, &height, &channels, reqComp); 
	size = width * height * reqComp;
	if (!pixels || !size) {
		char err[256];
		sprintf(err, "TEXTURE ERROR reason: %s path:\"%s\" w:%d h:%d channels:%d reqComp:%d ", stbi_failure_reason(), filename, width, height, channels, reqComp); 
		throw std::runtime_error(err);
	}
	
	LOG("TEXTURE LOADED path:\""<< filename << "\" w:" << width << " h:" << height << " channels:" << channels << " reqComp:"<< reqComp << " size:" << size);
	return pixels;
}

int TextureData::getWidth() const 
{
	return width;
}

int TextureData::getHeight() const 
{
	return height;
}

int TextureData::getChannels() const
{
	return channels;
}

stbi_uc* TextureData::getPixels()
{
	return pixels;
}

uint64_t TextureData::getSize() const 
{
	return size;
}

TextureDesc::TextureDesc(const char* filename, int reqComp): 
TextureDesc(std::string(filename), reqComp)  
{
	
}

TextureDesc::TextureDesc(std::string filename, int reqComp):
   filename(filename), reqComp(reqComp)	
{

}

bool TextureDesc::operator==(const TextureDesc &other) const
{
	return filename == other.filename && reqComp == other.reqComp;
}

