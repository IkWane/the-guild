#pragma once
#include <vector>
#include <fstream>
#include "magicNumbers.hpp"
#include "Adventurer.hpp"
#include "Mission.hpp"
#include "json.hpp"
#include "optional"

class TheGuild
{
public:
    TheGuild();
    TheGuild(std::string &path);
    int gold;
    std::vector<Adventurer> adventurers;
    std::vector<Mission> missions;
    void saveGuild(const char *path);
    std::optional<Adventurer*> getAdventurerByName(std::string &name);
    std::vector<std::string> adventurerNames();
};