#include "Mission.hpp"

// Loads mission from JSON data
Mission::Mission(nlohmann::json json)
{
    Debug::dbg << "Creating mission from JSON\n";
    description = json["description"].get<std::string>();
    monsters = json["monsters"].get<std::map<std::string, int>>();
    terrainType = json["terrain_type"].get<std::string>();
    level = json["level"].get<int>();
    reward = json["reward"].get<int>();
    assignedAdventurers = json.contains("assigned_adventurers") ? json["assigned_adventurers"].get<std::vector<std::string>>() : std::vector<std::string>();
    identifier = json["identifier"].get<std::string>();
}

// Converts Mission to JSON data
nlohmann::json Mission::toJson()
{
    Debug::dbg << "Converting mission to JSON\n";
    nlohmann::json::object_t obj_values = {
        {"description", description},
        {"monsters", monsters},
        {"terrain_type", terrainType},
        {"level", level},
        {"reward", reward},
        {"assigned_adventurers", assignedAdventurers},
        {"identifier", identifier}
    };
    return nlohmann::json(obj_values);
}

void Mission::assignAdventurer(Adventurer &adv)
{
    assignedAdventurers.push_back(adv.identifier);
    adv.occupied = true;
}

void Mission::createIdentifier()
{
    std::string newIdentifier = (
        std::to_string(level) +
        terrainType
    );
    for (const auto &[key, value] : monsters)
    {
        for (int i = 0; i < value; i++)
        {
            newIdentifier += key;
        }
    }
    identifier = newIdentifier;
}
