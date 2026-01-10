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
    std::vector<Adventurer> adventurers;
    std::vector<Mission> missions;
    void saveGuild(const char *path);
    std::optional<Adventurer*> getAdventurerByIdentifier(std::string &identifier);
    void removeAdventurerByIdentifier(std::string &identifier);
    void removeMissionByIdentifier(Mission &mission);
    std::vector<std::string> adventurerIdentifiers();
    void addMission(Mission mission);
};