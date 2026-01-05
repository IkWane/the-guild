#pragma once
#include <exception>

class GameExit : public std::exception
{
public:
    int value;
    GameExit(int val): value(val) {}
};