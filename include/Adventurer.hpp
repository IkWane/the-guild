#pragma once
#include <string>
#include <vector>
#include "json.hpp"

class Adventurer
{
public:
    Adventurer(std::string name, int hunger)
        : name(name), hunger(hunger) {};
    Adventurer(nlohmann::json json);
    std::string name;
    int hunger;
    std::vector<std::string> weaknesses;
    std::vector<std::string> strengths;
};
