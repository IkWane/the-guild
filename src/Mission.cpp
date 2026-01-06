#include "Mission.hpp"

// Loads mission from JSON data
Mission::Mission(nlohmann::json json)
{
    Debug::dbg << "Creating mission from JSON\n";
    description = json["description"].get<std::string>();
    monsters = json["monsters"].get<std::map<std::string, int>>();
    terrainType = json["terrain_type"].get<std::string>();
    level = json["level"].get<int>();
    assignedAdventurers = json.contains("assigned_adventurers") ? json["assigned_adventurers"].get<std::vector<std::string>>() : std::vector<std::string>();
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
        {"assigned_adventurers", assignedAdventurers}
    };
    return nlohmann::json(obj_values);
}

// Creates a mission card (text-based) for the mission
// Output is a vector of strings, each string being a line
std::vector<std::string> Mission::toMissionCard()
{
    const int len = 30;
    Debug::dbg << "Creating mission card for mission: " << description << "\n";
    std::vector<std::string> card = {
             gameUtil::fitStr("/", len-1, '-') + "\\",
        gameUtil::fitStr("| Mission: " + description, len-1) + "|",
        gameUtil::fitStr("| Level: " + std::to_string(level), len-1) + "|",
        gameUtil::fitStr("| Terrain: " + terrainType, len-1) + "|",
        gameUtil::fitStr("| Monsters:", len-1) + "|"
    };
    for (auto &key : monsters_keys)
    {
        card.push_back(gameUtil::fitStr("|  " + gameUtil::snakeToNormal(key, true) + " x" + std::to_string(monsters[key]), len-1) + "|");
    }
    card.push_back(gameUtil::fitStr("\\", len-1, '-') + "/");
    return card;
}
