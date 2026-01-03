#include <string>
#include <vector>
#include "json.hpp"

class Mission
{
public:
    Mission(std::string description, int level, std::map<std::string, int> monsters, std::vector<std::string> monsters_keys):
        description(description), monsters(monsters), monsters_keys(monsters_keys), terrainType("None"), level(level) {};
    Mission(nlohmann::json json);
    nlohmann::json toJson();
    std::string description;
    std::map<std::string, int> monsters;
    std::vector<std::string> monsters_keys;
    std::string terrainType;
    int level;
};