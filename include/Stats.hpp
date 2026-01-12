#pragma once
#include <vector>
#include <math.h>
#include "Debug.hpp"

struct Stats
{
    int strength = 0;
    int agility = 0;
    int fortitude = 0;
    int willpower = 0;
    int perception = 0;
    int wisdom = 0;
    int magic = 0;
    int add_strength = 0;
    int add_agility = 0;
    int add_fortitude = 0;
    int add_willpower = 0;
    int add_perception = 0;
    int add_wisdom = 0;
    int add_magic = 0;
    std::vector<std::string> strengths = std::vector<std::string>();
    std::vector<std::string> weaknesses = std::vector<std::string>();
    int calculateLevel();
    void balanceStrengthsAndWeaknesses();
};
