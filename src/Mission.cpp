#include "Mission.hpp"

Mission::Mission(nlohmann::json json)
{
    description = json["description"].get<std::string>();
    monsters = json["monsters"].get<std::map<std::string, int>>();
    terrainType = json["terrain_type"].get<std::string>();
    level = json["level"].get<int>();
}

nlohmann::json Mission::toJson()
{
    nlohmann::json::object_t obj_values = {
        {"description", description},
        {"monsters", monsters},
        {"terrain_type", terrainType},
        {"level", level}
    };
    return nlohmann::json(obj_values);
}
