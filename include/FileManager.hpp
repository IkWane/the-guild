#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include "json.hpp"
#include "gameUtil.hpp"
#include "Debug.hpp"

// All paths are relative to the executable file
class FileManager
{
public:
    static std::string loadFile(const char *path);
    static std::string loadFile(std::string& path);
    static nlohmann::json loadJson(const char *path);
    static nlohmann::json loadJson(std::string& path);
    static std::vector<std::string> getFilesInDirectory(const char *path);
};