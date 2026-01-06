#include <string>
#include <vector>
#include "json.hpp"
#include "Debug.hpp"
#include "gameUtil.hpp"

class Mission
{
public:
    Mission(std::string description, int level, std::map<std::string, int> monsters, std::vector<std::string> monsters_keys):
        description(description), monsters(monsters), monsters_keys(monsters_keys), terrainType("None"), level(level), assignedAdventurers(std::vector<std::string>()) {};
    Mission(nlohmann::json json);
    nlohmann::json toJson();
    std::string description;
    std::map<std::string, int> monsters;
    std::vector<std::string> monsters_keys;
    std::string terrainType;
    std::vector<std::string> assignedAdventurers;
    int level;
    std::vector<std::string> toMissionCard();
};