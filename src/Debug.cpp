#include "Debug.hpp"

Debug Debug::dbg = Debug();

// Initializes the debug logger by setting the log file path
void Debug::initDebug(const char *path)
{
    std::ofstream file(path);
    file << "";
    file.close();
    dbg.path = path;
}

// Default constructor sets empty path
Debug::Debug()
{
    this->path = "";
}