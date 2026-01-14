#pragma once
#include <string>
#include <vector>
#include "Adventurer.hpp"
#include "json.hpp"

class Mission
{
public:
    Mission(std::string description, int level, int reward, int days, std::map<std::string, int> monsters):
        description(description), monsters(monsters), terrainType("None"), level(level), days(days), reward(reward), assignedAdventurers(std::vector<std::string>()) {};
    Mission(nlohmann::json json);
    nlohmann::json toJson();
    std::string description;
    std::map<std::string, int> monsters;
    std::string terrainType;
    std::vector<std::string> assignedAdventurers;
    void assignAdventurer(Adventurer &adv);
    void removeAdventurer(std::string identifier);
    bool isFull();
    void createIdentifier();
    int days;
    int level;
    int reward;
    std::string identifier;
};