#include "Debug.hpp"

Debug Debug::dbg = Debug();

void Debug::initDebug(const char *path)
{
    dbg.path = path;
}

Debug::Debug()
{
    this->path = "";
}