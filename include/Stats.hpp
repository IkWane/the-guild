#pragma once
#include <vector>

struct Stats
{
    int strength = 0;
    int agility = 0;
    int fortitude = 0;
    int willpower = 0;
    int perception = 0;
    int wisdom = 0;
    int magic = 0;
    std::vector<std::string> strengths = std::vector<std::string>();
    std::vector<std::string> weaknesses = std::vector<std::string>();
};
