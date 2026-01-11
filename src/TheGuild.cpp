#include "TheGuild.hpp"

TheGuild::TheGuild()
{
    this->name = "New Guild";
    gold = STARTER_GOLD;
    adventurers = std::vector<Adventurer>();
    missions = std::vector<Mission>();
    phase = 0;
    day = 0;
    rations = 0;
}

// Initializes a new guild with starter gold and empty adventurer and mission lists
TheGuild::TheGuild(std::string name)
{
    this->name = name;
    gold = STARTER_GOLD;
    phase = 0;
    day = 0;
    rations = 0;
    adventurers = std::vector<Adventurer>();
    missions = std::vector<Mission>();
}

// Loads the guild data from a JSON file at the given path
TheGuild::TheGuild(const char *path)
{
    nlohmann::json jsonGuild = FileManager::loadJson(path);
    name = jsonGuild["name"].get<std::string>();
    gold = jsonGuild["gold"].get<int>();
    phase = jsonGuild["phase"].get<int>();
    day = jsonGuild["day"].get<int>();
    rations = jsonGuild["rations"].get<int>();
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
    jsonGuild["name"] = name;
    jsonGuild["gold"] = gold;
    jsonGuild["phase"] = phase;
    jsonGuild["day"] = day;
    jsonGuild["rations"] = rations;
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

bool TheGuild::hasLost()
{
    return false;
}

// Returns an adventurer pointer if found, else returns empty optional.
// (Note: returning pointer to element in vector, so be careful with vector modifications)
std::optional<Adventurer*> TheGuild::getAdventurerByIdentifier(std::string &identifier)
{
    Debug::dbg << "Searching for adventurer by identifier: '" << identifier << "'...";
    for (auto &el : adventurers)
    {
        if (el.identifier == identifier)
        {
            Debug::dbg << "found!\n";
            return &el;
        }
    }
    Debug::dbg << "not found.\n";
    return {};
}

void TheGuild::removeAdventurerByIdentifier(std::string &identifier)
{
    Debug::dbg << "trying to remove adventurer by identifier: '" << identifier << "'...";
    for (int i = 0; i < adventurers.size(); i++)
    {
        if (adventurers[i].identifier == identifier)
        {
            adventurers.erase(adventurers.begin() + i);
            Debug::dbg << "removed!\n";
            return;
        }
    }
    Debug::dbg << "not found.\n";
}

void TheGuild::removeMissionByIdentifier(Mission &mission)
{
    Debug::dbg << "trying to remove mission...";
    int i = 0;
    for (auto &otherMission : missions)
    {
        if (otherMission.identifier == mission.identifier)
        {
            missions.erase(missions.begin() + i);
            Debug::dbg << "removed!\n";
            return;
        }
        i++;
    }
    
    Debug::dbg << "not found.\n";
}

// Returns a vector of all adventurer names in the guild
std::vector<std::string> TheGuild::adventurerIdentifiers()
{
    std::vector<std::string> names;
    for (const auto &adv : adventurers)
    {
        names.push_back(adv.identifier);
    }
    return names;
}

void TheGuild::addMission(Mission mission)
{
    Debug::dbg << "Adding mission to guild\n";
    missions.push_back(mission);
    for (auto &advName : mission.assignedAdventurers)
    {
        std::optional<Adventurer*> advOptional = getAdventurerByIdentifier(advName);
        if (advOptional.has_value())
        {
            advOptional.value()->occupied = true;
            Debug::dbg << advName << "is now occupied\n";
        }
    }
    
}

// Returns a vector of pointers to unoccupied adventurers in the guild
// Note : modifying the adventurers vector causes these pointers to become erroneous
std::vector<Adventurer*> TheGuild::getUnoccupiedAdventurers()
{
    std::vector<Adventurer*> unoccupied_adventurers = std::vector<Adventurer *>();
    for (auto &adv : adventurers)
    {
        if (!adv.occupied)
        {
            unoccupied_adventurers.push_back(&adv);
        }
    }
    
    return unoccupied_adventurers;
}
