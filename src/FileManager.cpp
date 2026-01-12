#include "FileManager.hpp"

// Loads the content of a file into a string
std::string FileManager::loadFile(const char *path)
{
    Debug::dbg << "Loading file: " << path << "\n";
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw GameExit("Error loading file " + std::string(path));
    }
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
    try {
        nlohmann::json result = nlohmann::json::parse(fileContent);
        return result;
    }
    catch (nlohmann::json::parse_error &e) {
        throw GameExit("Error parsing JSON of file " + std::string(path));
    }
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

std::string FileManager::getDirectory(std::string path)
{
    std::string directory;
    size_t last_slash = path.rfind("/");
    if (std::string::npos != last_slash)
    {
        directory = path.substr(0, last_slash);
    }
    
    return directory;
}