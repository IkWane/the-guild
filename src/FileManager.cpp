#include "FileManager.hpp"

// Loads the content of a file into a string
std::string FileManager::loadFile(const char *path)
{
    Debug::dbg << "Loading file: " << path << "\n";
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

// Loads the content of a file into a string
// Overload to accept std::string path
std::string FileManager::loadFile(std::string &path)
{
    return loadFile(path.c_str());
}

// Loads and parses a JSON file from the given path
nlohmann::json FileManager::loadJson(const char *path)
{
    std::string fileContent = loadFile(path);
    Debug::dbg << "Parsing JSON\n";
    nlohmann::json result = nlohmann::json::parse(fileContent);
    return result;
}

// Loads and parses a JSON file from the given path
// Overload to accept std::string path
nlohmann::json FileManager::loadJson(std::string &path)
{
    return loadJson(path.c_str());
}

// Gets the names of all files in the given directory
std::vector<std::string> FileManager::getFilesInDirectory(const char *path)
{
    std::vector<std::string> file_names;
    for (auto file : std::filesystem::directory_iterator(path))
    {
        file_names.push_back(file.path().string());
    }
    return file_names;
}
