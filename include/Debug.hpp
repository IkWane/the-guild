#pragma once
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>
// #include "FileManager.hpp"

class Debug {
public:
    static void initDebug(const char* path);
    static Debug dbg;
    template<typename T> friend Debug& operator<<(Debug& d, T t)
    {
        if (d.path == "")
        {
            std::cerr << "Debug was not initialized properly" << std::endl;
            return d;
        }
        std::ofstream file(d.path.c_str(), std::ios_base::app | std::ios_base::out);
        file << t;
        file.close();
        return d;
    }
private:
    std::string path;
    Debug();
};
