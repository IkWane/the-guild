#pragma once
#include <string>
#include <vector>
#include "json.hpp"

class Adventurer
{
public:
    Adventurer(std::string name, int hunger): 
        name(name), 
        hunger(hunger), 
        strength(0), 
        agility(0), 
        fortitude(0), 
        willpower(0), 
        perception(0), 
        wisdom(0), 
        magic(0) 
        {};
    Adventurer(nlohmann::json json);
    nlohmann::json toJson();
    std::string name;
    int hunger;
    int strength;
    int agility;
    int fortitude;
    int willpower;
    int perception;
    int wisdom;
    int magic;
    std::vector<std::string> weaknesses;
    std::vector<std::string> strengths;
};
