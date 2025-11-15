#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"

// All paths are relative to the executable file
class FileManager
{
public:
    static std::string loadFile(const char *path);
    static nlohmann::json loadJson(const char *path);
};