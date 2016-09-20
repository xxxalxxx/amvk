#ifndef AMVK_FILE_MANAGER
#define AMVK_FILE_MANAGER

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <limits.h>
#include <stdio.h>
#include <macro.h>
#include <stdexcept>

#ifdef WINDOWS
    #include <direct.h>
    #define getCurrentDir _getcwd
#else
    #include <unistd.h>
    #define getCurrentDir getcwd
#endif


class FileManager {
public:
	static FileManager& getInstance();
	std::string getBinPath();
	std::vector<char> readFile(const std::string& filename); 
	std::vector<char> readShader(const std::string& shaderName);
	std::string getFilePath(const std::string& filename);
	FileManager(const FileManager& fileManager) = delete;
	void operator=(const FileManager& fileManager) = delete;
private:
	FileManager();
	void initBinPath();

	std::string mBinPath, mEngineRoot, mShaderDir;
	static constexpr const char* ENGINE_RELATIVE_ROOT = "/../";
	static constexpr const char* SHADER_DIR = "shader/spv/";

};

#endif
