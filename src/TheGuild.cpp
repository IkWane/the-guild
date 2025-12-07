#include "TheGuild.hpp"

TheGuild::TheGuild()
{
    gold = STARTER_GOLD;
}

TheGuild::TheGuild(std::string &path)
{
    std::ifstream file(path.c_str());
    assert(!file.fail() && "Failed to open guild save file.");
    nlohmann::json jsonGuild;
    file >> jsonGuild;
    gold = jsonGuild["gold"].get<int>();
    for (nlohmann::json jsonAdv : jsonGuild["adventurers"])
    {
        Adventurer adv(
            jsonAdv["name"].get<std::string>(),
            jsonAdv["healthPoints"].get<int>()
        );
        adv.strength = jsonAdv["strength"].get<int>();
        adv.agility = jsonAdv["agility"].get<int>();
        adv.fortitude = jsonAdv["fortitude"].get<int>();
        adv.willpower = jsonAdv["willpower"].get<int>();
        adv.perception = jsonAdv["perception"].get<int>();
        adv.wisdom = jsonAdv["wisdom"].get<int>();
        adv.magic = jsonAdv["magic"].get<int>();
        adv.weaknesses = jsonAdv["weaknesses"].get<std::vector<std::string>>();
        adv.strengths = jsonAdv["strengths"].get<std::vector<std::string>>();
        adventurers.push_back(adv);
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
    file << jsonGuild;
    file.close();
}
