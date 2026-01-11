#pragma once
#include <vector>
#include <fstream>
#include "magicNumbers.hpp"
#include "Adventurer.hpp"
#include "json.hpp"
#include "Mission.hpp"
#include "optional"

class TheGuild
{
public:
    TheGuild();
    TheGuild(std::string name);
    TheGuild(const char *path);
    std::string name;
    int phase;
    int gold;
    int rations;
    int day;
    std::vector<Adventurer> adventurers; //? should use a map with identifiers as keys, will change if I have time
    std::vector<Mission> missions;
    void saveGuild(const char *path);
    bool hasLost();
    std::optional<Adventurer*> getAdventurerByIdentifier(std::string &identifier);
    void removeAdventurerByIdentifier(std::string &identifier);
    void removeMissionByIdentifier(Mission &mission);
    std::vector<std::string> adventurerIdentifiers();
    void addMission(Mission mission);
    std::vector<Adventurer*> getUnoccupiedAdventurers();
};