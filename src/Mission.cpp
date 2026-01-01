#include "Mission.hpp"

Mission::Mission(nlohmann::json json)
{
    description = json["description"].get<std::string>();
    monsters = json["monsters"].get<std::vector<std::string>>();
    terrainType = json["terrain_type"].get<std::string>();
    rank = json["rank"].get<int>();
}

nlohmann::json Mission::toJson()
{
    nlohmann::json::object_t obj_values = {
        {"description", description},
        {"monsters", monsters},
        {"terrain_type", terrainType},
        {"rank", rank}
    };
    return nlohmann::json(obj_values);
}
