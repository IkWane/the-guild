#pragma once
#include <string>
#include <vector>
#include "json.hpp"
#include "Debug.hpp"
#include "gameUtil.hpp"

class Mission
{
public:
    Mission(std::string description, int level, int reward, std::map<std::string, int> monsters):
        description(description), monsters(monsters), terrainType("None"), level(level), reward(reward), assignedAdventurers(std::vector<std::string>()) {};
    Mission(nlohmann::json json);
    nlohmann::json toJson();
    std::string description;
    std::map<std::string, int> monsters;
    std::string terrainType;
    std::vector<std::string> assignedAdventurers;
    void assignAdventurer(Adventurer &adv);
    void createIdentifier();
    int level;
    int reward;
    std::string identifier;
};