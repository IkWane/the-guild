#include <string>
#include <vector>
#include "json.hpp"

class Mission
{
public:
    Mission(std::string description):
        description(description), monsters(std::map<std::string, int>()), terrainType("None"), rank(0) {};
    Mission(nlohmann::json json);
    nlohmann::json toJson();
    std::string description;
    std::map<std::string, int> monsters;
    std::string terrainType;
    int rank;
};