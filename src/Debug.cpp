#include "Debug.hpp"

Debug Debug::dbg = Debug();

void Debug::initDebug(const char *path)
{
    std::ofstream file(path);
    file << "";
    file.close();
    dbg.path = path;
}

Debug::Debug()
{
    this->path = "";
}