#include "Mission.hpp"

Mission::Mission(nlohmann::json json)
{
    Debug::dbg << "Creating mission from JSON\n";
    description = json["description"].get<std::string>();
    monsters = json["monsters"].get<std::map<std::string, int>>();
    terrainType = json["terrain_type"].get<std::string>();
    level = json["level"].get<int>();
    assignedAdventurers = json.contains("assigned_adventurers") ? json["assigned_adventurers"].get<std::vector<std::string>>() : std::vector<std::string>();
}

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
