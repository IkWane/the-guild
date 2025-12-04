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

std::string FileManager::loadFile(std::string &path)
{
    return loadFile(path.c_str());
}

nlohmann::json FileManager::loadJson(const char *path)
{
    std::string fileContent = loadFile(path);
    nlohmann::json result = nlohmann::json::parse(fileContent);
    return result;
}

nlohmann::json FileManager::loadJson(std::string &path)
{
    return loadJson(path.c_str());
}

std::vector<std::string> FileManager::getFilesInDirectory(const char *path)
{
    std::vector<std::string> file_names;
    for (auto file : std::filesystem::directory_iterator(path))
    {
        file_names.push_back(file.path().string());
    }
    return file_names;
}
