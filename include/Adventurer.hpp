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
    Adventurer(std::string name, int satiation): 
        name(name),
        level(0),
        satiation(satiation),
        stats(),
        modifiers(std::vector<std::string>()),
        occupied(false)
        {};
    Adventurer(nlohmann::json json);
    void resetStats();
    void updateLevel();
    void balanceStats();
    void createIdentifier();
    bool isStarved();
    int getSalary();
    nlohmann::json toJson();
    std::string name;
    int level;
    int satiation;
    Stats stats;
    std::string identifier;
    std::vector<std::string> modifiers;
    std::string gameClass;
    std::string race;
    bool occupied;
    std::vector<std::string> toCharacterCard();
};
