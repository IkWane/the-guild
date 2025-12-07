#include "Adventurer.hpp"

Adventurer::Adventurer(nlohmann::json json)
{
    name = json["name"].get<std::string>();
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

nlohmann::json Adventurer::toJson()
{
    nlohmann::json::object_t obj_values = {
        {"name", name},
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
