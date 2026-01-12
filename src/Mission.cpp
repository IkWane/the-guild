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
    days = json["days"].get<int>();
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
        {"days", days},
        {"assigned_adventurers", assignedAdventurers},
        {"identifier", identifier}
    };
    return nlohmann::json(obj_values);
}

void Mission::assignAdventurer(Adventurer &adv)
{
    if (!isFull())
    {
        Debug::dbg << "Assigned adventurer " << adv.name << " to mission\n"; 
        assignedAdventurers.push_back(adv.identifier);
        adv.occupied = true;
    }
}

void Mission::removeAdventurer(std::string identifier)
{
    Debug::dbg << "trying to remove adventurer by identifier: '" << identifier << "'...";
    for (int i = 0; i < assignedAdventurers.size(); i++)
    {
        if (assignedAdventurers[i] == identifier)
        {
            assignedAdventurers.erase(assignedAdventurers.begin() + i);
            Debug::dbg << "removed!\n";
            return;
        }
    }
    Debug::dbg << "not found.\n";
}

bool Mission::isFull()
{
    return !(assignedAdventurers.size() < 4);
}

void Mission::createIdentifier()
{
    std::string newIdentifier = (
        std::to_string(level) +
        terrainType
    );
    for (const auto &[key, value] : monsters)
    {
        newIdentifier += key + std::to_string(value);
    }
    identifier = newIdentifier;
}
