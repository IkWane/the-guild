#pragma once
#include <string>
#include <vector>
#include "json.hpp"

class Adventurer
{
public:
    Adventurer(std::string name, int hunger): 
        name(name),
        level(0),
        hunger(hunger), 
        strength(0), 
        agility(0), 
        fortitude(0), 
        willpower(0), 
        perception(0), 
        wisdom(0), 
        magic(0),
        strengths(std::vector<std::string>()),
        weaknesses(std::vector<std::string>()),
        modifiers(std::vector<std::string>())
        {};
    Adventurer(nlohmann::json json);
    void resetStats();
    void updateLevel();
    nlohmann::json toJson();
    std::string name;
    int level;
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
    std::vector<std::string> modifiers;
    std::string gameClass;
    std::string race;
};
