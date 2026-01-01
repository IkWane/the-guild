#include "TheGuild.hpp"

TheGuild::TheGuild()
{
    gold = STARTER_GOLD;
    adventurers = std::vector<Adventurer>();
    missions = std::vector<Mission>();
}

TheGuild::TheGuild(std::string &path)
{
    std::ifstream file(path.c_str());
    assert(!file.fail() && "Failed to open guild save file.");
    nlohmann::json jsonGuild;
    file >> jsonGuild;
    gold = jsonGuild["gold"].get<int>();
    for (nlohmann::json &jsonAdv : jsonGuild["adventurers"])
    {
        Adventurer adv(jsonAdv);
        adventurers.push_back(adv);
    }
    for (auto &jsonMission : jsonGuild["missions"])
    {
        Mission mission(jsonMission);
        missions.push_back(mission);
    }
    
}

void TheGuild::saveGuild(const char *path)
{
    std::ofstream file(path);
    nlohmann::json jsonGuild;
    jsonGuild["gold"] = gold;
    jsonGuild["adventurers"] = nlohmann::json::array();
    for (Adventurer &adv : adventurers)
    {
        nlohmann::json jsonAdv = adv.toJson();
        jsonGuild["adventurers"].push_back(jsonAdv);
    }
    jsonGuild["missions"] = nlohmann::json::array();
    for (auto &mission : missions)
    {
        nlohmann::json jsonMission = mission.toJson();
        jsonGuild["missions"].push_back(jsonMission);
    }
    
    file << jsonGuild.dump();
    file.close();
}
