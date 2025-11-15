#include "FileManager.hpp"

std::string FileManager::loadFile(const char *path)
{
    std::ifstream file(path);
    std::string lineStr;
    std::string result;
    while (std::getline(file, lineStr))
    {
        result = result + lineStr + "\n";
    }
    file.close();
    return result;
}

nlohmann::json FileManager::loadJson(const char *path)
{
    std::string fileContent = loadFile(path);
    nlohmann::json result = nlohmann::json::parse(fileContent);
    return result;
}
