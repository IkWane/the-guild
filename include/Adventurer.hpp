#pragma once
#include <string>
#include <vector>

class Adventurer
{
public:
    Adventurer(std::string name, int hunger)
        : name(name), hunger(hunger) {};
    ~Adventurer();
    std::string name;
    int hunger;
    std::vector<std::string> weaknesses;
    std::vector<std::string> strengths;
};
