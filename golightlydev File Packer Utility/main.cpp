#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <filesystem>

struct FileData {
	std::string id;
	size_t fileStart;
	size_t fileSize;
	std::string fileType;
	uint32_t textureWidth;
	uint32_t textureHeight;
	int subTextureNum;
	std::string* subTextureID;
	uint32_t* subTextureStartX;
	uint32_t* subTextureStartY;
	uint32_t* subTextureWidth;
	uint32_t* subTextureHeight;
};

void writeResourceContainer(FileData* fileData, std::string& resourceDataContainerFileType, std::ofstream& writeResourceDataContainerFile, size_t fileNum) {
	size_t index = 0;
	const char* getRunningData_0 = R"(#include "../gldev_engineHeaders/gldev_engineConditional.h"
#include "../gldev_programHeaders/gldev_resourceDataContainer.h"
#include "../gldev_engineHeaders/gldev_size_t_withErrorReportCapability.h"
#include <cstdlib>
#include <string>

int gldev::ResourceDataContainer::getRunningData(int resourceType, size_t resourceID, int subTextureID, std::string field) {
	switch (resourceType) {
	case GLDEV_RESOURCE_TYPE_TEXTURE:
		switch (resourceID) {)";
	writeResourceDataContainerFile << getRunningData_0 << std::endl;
	for ( ; index < fileNum && fileData[index].fileType == "texture"; ++index) {
		writeResourceDataContainerFile << "		case " << index << " : //" << fileData[index].id << std::endl;
		for (int a = 0; a < fileData[index].subTextureNum; ++a) {
			writeResourceDataContainerFile << "			switch (subTextureID) {" << std::endl;
			writeResourceDataContainerFile << "			case " << a << " : //" << fileData[index].subTextureID[a] << std::endl;
			writeResourceDataContainerFile << "				if (field == \"subTextureStartX\")" << std::endl;
			writeResourceDataContainerFile << "					return " << fileData[index].subTextureStartX[a] << ";" << std::endl;
			writeResourceDataContainerFile << "				else if (field == \"subTextureStartY\")" << std::endl;
			writeResourceDataContainerFile << "					return " << fileData[index].subTextureStartY[a] << ";" << std::endl;
			writeResourceDataContainerFile << "				else if (field == \"subTextureWidth\")" << std::endl;
			writeResourceDataContainerFile << "					return " << fileData[index].subTextureWidth[a] << ";" << std::endl;
			writeResourceDataContainerFile << "				else if (field == \"subTextureHeight\")" << std::endl;
			writeResourceDataContainerFile << "					return " << fileData[index].subTextureHeight[a] << ";" << std::endl;
			writeResourceDataContainerFile << "				break;" << std::endl;
		}
		const char* getRunningData_1 = R"(			default:
				return -1;
				break;
			})";
		writeResourceDataContainerFile << getRunningData_1 << std::endl;
		writeResourceDataContainerFile << "			break;" << std::endl;
	}

	const char* getRunningData_2 = R"(		default:
			return -1;
			break;
		})";
	writeResourceDataContainerFile << getRunningData_2 << std::endl;

	//other data types go here...

	const char* getRunningData_3 = R"(		break;
	default:
		return -1;
		break;
	}
}

)";
	writeResourceDataContainerFile << getRunningData_3;
	index = 0;
	const char* getLoadingData_0 = R"(gldev::Size_t_withErrorReportCapability gldev::ResourceDataContainer::getLoadingData(int resourceType, size_t resourceID, std::string field) {
	switch (resourceType) {
	case GLDEV_RESOURCE_TYPE_TEXTURE :
		switch (resourceID) {)";
	writeResourceDataContainerFile << getLoadingData_0 << std::endl;
	for (; index < fileNum && fileData[index].fileType == "texture"; ++index) {
		writeResourceDataContainerFile << "		case " << index << " : //" << fileData[index].id << std::endl;
		writeResourceDataContainerFile << "			if (field == \"resourceDataStart\")" << std::endl;
		writeResourceDataContainerFile << "				return gldev::Size_t_withErrorReportCapability(" << fileData[index].fileStart << ", false);" << std::endl;
		writeResourceDataContainerFile << "			else if (field == \"resourceDataSize\")" << std::endl;
		writeResourceDataContainerFile << "				return gldev::Size_t_withErrorReportCapability(" << fileData[index].fileSize << ", false);" << std::endl;
		writeResourceDataContainerFile << "			break;" << std::endl;
	}
	const char* getLoadingData_1 = R"(		default:
			return gldev::Size_t_withErrorReportCapability(0, true);
			break;
		}
		break;)";
	writeResourceDataContainerFile << getLoadingData_1 << std::endl;

	//other data types go here...

	const char* getLoadingData_2 = R"(	default:
		return gldev::Size_t_withErrorReportCapability(0, true);
		break;
	}
})";
	writeResourceDataContainerFile << getLoadingData_2 << std::endl;
	writeResourceDataContainerFile.close();
	writeResourceDataContainerFile.clear();
}

int main() {
	std::string dataCollectionType; //manual or automatic
	std::string directoryPath;
	std::string writeFilePath;
	std::string resourceDataContainerFilePath;
	size_t fileNum = 0;
	FileData* fileData;
	std::ofstream writeFile;
	std::ofstream writeResourceDataContainerFile;
	char* file;
	std::string resourceDataContainerFileType;
	std::cout << "data collection type (manual or automatic): ";
	std::cin >> dataCollectionType;
	if (dataCollectionType != "manual" && dataCollectionType != "automatic")
		return 0;
	std::cout << "file type(sdl2, vulkan, directx12, opengl): ";
	std::cin >> resourceDataContainerFileType;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "complete directory path (add slash at the end): ";
	std::getline(std::cin, directoryPath);
	writeFilePath = directoryPath;
	resourceDataContainerFilePath = directoryPath;
	resourceDataContainerFilePath += u8"gldev_resourceDataContainer.cpp";
	writeFilePath += u8"resources.gldev";
	if (dataCollectionType == "manual") {
		std::cout << "number of files: ";
		std::cin >> fileNum;
	}
	else if (dataCollectionType == "automatic") {
		for (auto& file : std::filesystem::directory_iterator(((std::string)(directoryPath + "texture")).c_str())) {
			if(file.is_regular_file())
				++fileNum;
		}

		//other resource types here...

	}
	writeFile.open(writeFilePath.c_str(), std::ios::binary);
	fileData = new FileData[fileNum];
	if (dataCollectionType == "manual") {
		for (size_t a = 0; a < fileNum; ++a) {
			std::string readFilePath;
			std::cout << a << ") file name: ";
			std::string id;
			std::cin >> id;
			readFilePath = directoryPath;
			readFilePath += id;
			fileData[a].id = id;
			std::ifstream readFile(readFilePath.c_str(), std::ios::binary | std::ios::ate);
			std::cout << a << ") file type: ";
			std::cin >> fileData[a].fileType;
			if (fileData[a].fileType == "texture") {
				std::cout << a << ") texture width: ";
				std::cin >> fileData[a].textureWidth;
				std::cout << a << ") texture height: ";
				std::cin >> fileData[a].textureHeight;
				std::cout << a << ") subTexture num: ";
				std::cin >> fileData[a].subTextureNum;
				fileData[a].subTextureID = new std::string[fileData[a].subTextureNum];
				fileData[a].subTextureStartX = new uint32_t[fileData[a].subTextureNum];
				fileData[a].subTextureStartY = new uint32_t[fileData[a].subTextureNum];
				fileData[a].subTextureWidth = new uint32_t[fileData[a].subTextureNum];
				fileData[a].subTextureHeight = new uint32_t[fileData[a].subTextureNum];
				for (int b = 0; b < fileData[a].subTextureNum; ++b) {
					std::cout << a << "-" << b << ") subTexture id: ";
					std::cin >> fileData[a].subTextureID[b];
					std::cout << a << "-" << b << ") subTexture start x: ";
					std::cin >> fileData[a].subTextureStartX[b];
					std::cout << a << "-" << b << ") subTexture start y: ";
					std::cin >> fileData[a].subTextureStartY[b];
					std::cout << a << "-" << b << ") subTexture width: ";
					std::cin >> fileData[a].subTextureWidth[b];
					std::cout << a << "-" << b << ") subTexture height: ";
					std::cin >> fileData[a].subTextureHeight[b];
				}
			}
			fileData[a].fileStart = writeFile.tellp();
			fileData[a].fileSize = readFile.tellg();
			file = new char[fileData[a].fileSize];
			readFile.seekg(0);
			readFile.read(file, fileData[a].fileSize);
			readFile.close();
			readFile.clear();
			writeFile.write(file, fileData[a].fileSize);
			delete[] file;
		}
	}
	else if (dataCollectionType == "automatic") {
		size_t index = 0;
		std::ifstream readSupplementalData(((std::string)(directoryPath + u8"subTextureData.txt")).c_str());
		for (auto& directoryFile : std::filesystem::directory_iterator(((std::string)(directoryPath + u8"texture")).c_str())) {
			bool firstRun = true;
			std::ifstream readFile(directoryFile.path().u8string().c_str(), std::ios::binary | std::ios::ate);
			fileData[index].fileType = "texture";
			fileData[index].fileSize = readFile.tellg();
			fileData[index].id = directoryFile.path().stem().u8string();
			readFile.seekg(16);
			readFile.read(reinterpret_cast<char*>(&fileData[index].textureWidth), sizeof(uint32_t));
			readFile.read(reinterpret_cast<char*>(&fileData[index].textureHeight), sizeof(uint32_t));
			readFile.seekg(0);
			fileData[index].fileStart = writeFile.tellp();
			file = new char[fileData[index].fileSize];
			readFile.read(file, fileData[index].fileSize);
			readFile.close();
			readFile.clear();
			writeFile.write(file, fileData[index].fileSize);
			delete[] file;
			std::string input;
			if (firstRun)
				firstRun = false;
			else
				readSupplementalData >> input; //reads blank line
			readSupplementalData >> input; //read "texture#"
			readSupplementalData >> input; //read "subTextureNum:"
			readSupplementalData >> input; //read subTextureNum
			fileData[index].subTextureNum = atoi(input.c_str());
			fileData[index].subTextureID = new std::string[fileData[index].subTextureNum];
			fileData[index].subTextureStartX = new uint32_t[fileData[index].subTextureNum];
			fileData[index].subTextureStartY = new uint32_t[fileData[index].subTextureNum];
			fileData[index].subTextureWidth = new uint32_t[fileData[index].subTextureNum];
			fileData[index].subTextureHeight = new uint32_t[fileData[index].subTextureNum];
			for (int a = 0; a < fileData[index].subTextureNum; ++a) {
				readSupplementalData >> input; //read "subTexture#"
				readSupplementalData >> input; //read "subTextureID:"
				readSupplementalData >> fileData[index].subTextureID[a]; //read subTextureID
				readSupplementalData >> input; //read "subTextureStartX:"
				readSupplementalData >> input; //read subTextureStartX
				fileData[index].subTextureStartX[a] = atoi(input.c_str());
				readSupplementalData >> input; //read "subTextureStartY:"
				readSupplementalData >> input; //read subTextureStartY
				fileData[index].subTextureStartY[a] = atoi(input.c_str());
				readSupplementalData >> input; //read "subTextureWidth:"
				readSupplementalData >> input; //read subTextureWidth
				fileData[index].subTextureWidth[a] = atoi(input.c_str());
				readSupplementalData >> input; //read "subTextureHeight:"
				readSupplementalData >> input; //read subTextureHeight
				fileData[index].subTextureHeight[a] = atoi(input.c_str());
			}
			++index;
		}
		readSupplementalData.close();
		readSupplementalData.clear();

		//other resource types here...

	}
	writeFile.close();
	writeFile.clear();
	writeResourceDataContainerFile.open(resourceDataContainerFilePath.c_str());
	writeResourceContainer(fileData, resourceDataContainerFileType, writeResourceDataContainerFile, fileNum);
	for (int a = 0; a < fileNum; ++a) {
		delete[] fileData[a].subTextureID;
		delete[] fileData[a].subTextureStartX;
		delete[] fileData[a].subTextureStartY;
		delete[] fileData[a].subTextureWidth;
		delete[] fileData[a].subTextureHeight;
	}
	return 0;
}