#include "Mission.hpp"

Mission::Mission(nlohmann::json json)
{
    description = json["name"];
    monsters = json["monsters"].get<std::vector<std::string>>();
    terrainType = json["terrain_type"].get<std::string>();
    rank = json["rank"];
}

std::string Mission::toJson()
{
    
    return std::string();
}
