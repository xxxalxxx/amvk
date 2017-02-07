#ifndef AMVK_TEXTURE_MANAGER_H
#define AMVK_TEXTURE_MANAGER_H
#include "file_manager.h"
#include "macro.h"
#include <stb/stb_image.h>

class TextureData {
public:
	TextureData();
	~TextureData();
	stbi_uc* load(const char* filename, int reqComp);
	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	uint64_t getSize() const;
	stbi_uc* getPixels();
private:
	int mWidth, mHeight, mChannels, mSize;
	stbi_uc* mPixels; 
};

class TextureManager {
public:
	TextureManager();
	~TextureManager();
private:

};


#endif
