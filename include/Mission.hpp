#include <string>
#include <vector>
#include "json.hpp"

class Mission
{
public:
    Mission(std::string description):
        description(description), monsters(std::vector<std::string>()), terrainType("None"), rank(0) {};
    Mission(nlohmann::json json);
    std::string toJson();
    std::string description;
    std::vector<std::string> monsters;
    std::string terrainType;
    int rank;
};