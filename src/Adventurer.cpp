#include "Adventurer.hpp"

// Creates a new adventurer using status from json
Adventurer::Adventurer(nlohmann::json json)
{
    name = json["name"].get<std::string>();
    level = json["level"].get<int>();
    modifiers = json["modifiers"].get<std::vector<std::string>>();
    gameClass = json["class"].get<std::string>();
    race = json["race"].get<std::string>();
    hunger = json["hunger"].get<int>();
    strength = json["strength"].get<int>();
    agility = json["agility"].get<int>();
    fortitude = json["fortitude"].get<int>();
    willpower = json["willpower"].get<int>();
    perception = json["perception"].get<int>();
    wisdom = json["wisdom"].get<int>();
    magic = json["magic"].get<int>();
    weaknesses = json["weaknesses"].get<std::vector<std::string>>();
    strengths = json["strengths"].get<std::vector<std::string>>();
}

// Resets the stats (numerical as well as strengths and weaknesses)
void Adventurer::resetStats()
{
    strength = 0;
    agility = 0;
    fortitude = 0;
    willpower = 0;
    perception = 0;
    wisdom = 0;
    magic = 0;
    strengths = std::vector<std::string>();
    weaknesses = std::vector<std::string>();
}

void Adventurer::updateLevel()
{
    int points = strength + agility + fortitude + willpower + perception + wisdom + magic + strengths.size() - weaknesses.size();
    level = (int)sqrt(points);
}

// Converts adventurer data to json format
nlohmann::json Adventurer::toJson()
{
    nlohmann::json::object_t obj_values = {
        {"name", name},
        {"modifiers", modifiers},
        {"level", level},
        {"race", race},
        {"class", gameClass},
        {"hunger", hunger},
        {"strength", strength},
        {"agility", agility},
        {"fortitude", fortitude},
        {"willpower", willpower},
        {"perception", perception},
        {"wisdom", wisdom},
        {"magic", magic},
        {"weaknesses", weaknesses},
        {"strengths", strengths}
    };
    return nlohmann::json(obj_values);
}
