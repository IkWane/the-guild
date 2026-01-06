#pragma once
#include <string>
#include <vector>
#include "json.hpp"
#include "Debug.hpp"
#include "gameUtil.hpp"
#include <algorithm>
#include "Stats.hpp"

class Adventurer
{
public:
    Adventurer(std::string name, int hunger): 
        name(name),
        level(0),
        hunger(hunger),
        stats(),
        modifiers(std::vector<std::string>())
        {};
    Adventurer(nlohmann::json json);
    void resetStats();
    void updateLevel();
    void balanceStats();
    void balanceStrengthsAndWeaknesses();
    nlohmann::json toJson();
    std::string name;
    int level;
    int hunger;
    Stats stats;
    std::vector<std::string> modifiers;
    std::string gameClass;
    std::string race;
    std::vector<std::string> toCharacterCard();
};
