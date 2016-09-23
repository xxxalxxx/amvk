#include "file_manager.h"

FileManager::FileManager() 
{
	initBinPath();
	mEngineRoot = mBinPath + ENGINE_RELATIVE_ROOT;
	mResourceDir = mEngineRoot + RESOURCE_DIR;
	mShaderDir = mEngineRoot + SHADER_DIR;
	LOG("SHADER DIR:" << mShaderDir);
}

FileManager& FileManager::getInstance()
{
	static FileManager fileManager;
	return fileManager;
}

std::string FileManager::getResourcePath(std::string&& path) const 
{
	return mResourceDir + path;
}


std::vector<char> FileManager::readShader(const std::string& shaderName)
{
	std::string filename = mShaderDir + shaderName;
	LOG("SHADER FILE NAME:" << filename);
	return readFile(filename);
}


std::vector<char> FileManager::readFile(const std::string& filename) 
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) 
		throw std::runtime_error("failed to open file!");

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}


std::string FileManager::getFilePath(const std::string& filename)
{
	return filename.substr(0, filename.find_last_of("\\/"));
}

void FileManager::initBinPath()
{
	#ifdef WINDOWS
	char buff[MAX_PATH];
	#else
	char buff[PATH_MAX];
	#endif
    size_t sz = sizeof(buff)-1;
    LOG("IN BIN ABS PATH");
    #ifdef WINDOWS
        if (!GetModuleFileName(NULL, buff, sz)) 
			throw std::runtime_error("Could not read read binary file directory");
    #else 
        ssize_t len;
        if ((len = readlink("/proc/self/exe", buff, sz)) == -1) 
			throw std::runtime_error("Could not read read binary file directory");    
        buff[len] = '\0';
    #endif
    LOG("B ABS:" << std::string(buff));
    mBinPath = std::string(buff);
	mBinPath = getFilePath(mBinPath);
}

std::string FileManager::getBinPath() 
{
	return mBinPath;
}
