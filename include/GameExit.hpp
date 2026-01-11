#pragma once
#include <exception>

class GameExit : public std::exception
{
public:
    std::string message;
    GameExit(std::string message): message(message) {}
    GameExit(): message("") {}
};