#include "TheGuild.hpp"

// Initializes a new guild with starter gold and empty adventurer and mission lists
TheGuild::TheGuild()
{
    gold = STARTER_GOLD;
    adventurers = std::vector<Adventurer>();
    missions = std::vector<Mission>();
}

// Loads the guild data from a JSON file at the given path
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

// Saves the guild data to a JSON file at the given path (overwrites existing file)
void TheGuild::saveGuild(const char *path)
{
    Debug::dbg << "Saving guild to file: " << path << "\n";
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
    
    file << jsonGuild.dump(2);
    file.close();
}

// Returns an adventurer pointer if found, else returns empty optional.
// (Note: returning pointer to element in vector, so be careful with vector modifications)
std::optional<Adventurer*> TheGuild::getAdventurerByName(std::string &name)
{
    Debug::dbg << "Searching for adventurer by name: '" << name << "'...";
    for (auto &el : adventurers)
    {
        if (el.name == name)
        {
            Debug::dbg << "found!\n";
            return &el;
        }
    }
    Debug::dbg << "not found.\n";
    return {};
}

// Returns a vector of all adventurer names in the guild
std::vector<std::string> TheGuild::adventurerNames()
{
    std::vector<std::string> names;
    for (const auto &adv : adventurers)
    {
        names.push_back(adv.name);
    }
    return names;
}