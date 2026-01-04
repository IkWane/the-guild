#include "Adventurer.hpp"

// Creates a new adventurer using status from json
Adventurer::Adventurer(nlohmann::json json)
{
    Debug::dbg << "Creating adventurer from JSON\n";
    name = json["name"].get<std::string>();
    level = json["level"].get<int>();
    modifiers = json["modifiers"].get<std::vector<std::string>>();
    gameClass = json["class"].get<std::string>();
    race = json["race"].get<std::string>();
    hunger = json["hunger"].get<int>();
    stats.strength = json["strength"].get<int>();
    stats.agility = json["agility"].get<int>();
    stats.fortitude = json["fortitude"].get<int>();
    stats.willpower = json["willpower"].get<int>();
    stats.perception = json["perception"].get<int>();
    stats.wisdom = json["wisdom"].get<int>();
    stats.magic = json["magic"].get<int>();
    stats.weaknesses = json["weaknesses"].get<std::vector<std::string>>();
    stats.strengths = json["strengths"].get<std::vector<std::string>>();
}

// Resets the stats (numerical as well as strengths and weaknesses)
void Adventurer::resetStats()
{
    Debug::dbg << "Resetting stats for adventurer: " << name << "\n";
    stats.strength = 0;
    stats.agility = 0;    
    stats.fortitude = 0;
    stats.willpower = 0;
    stats.perception = 0;
    stats.wisdom = 0;
    stats.magic = 0;
    stats.strengths = std::vector<std::string>();
    stats.weaknesses = std::vector<std::string>();
}

void Adventurer::updateLevel()
{
    Debug::dbg << "Updating level for adventurer: " << name << "\n";
    int points = (
        stats.strength + 
        stats.agility + 
        stats.fortitude + 
        stats.willpower + 
        stats.perception + 
        stats.wisdom + 
        stats.magic
    ) + stats.strengths.size() - stats.weaknesses.size();
    Debug::dbg << "Total stat points: " << points << "\n";
    level = int(sqrt(std::max(points, 1)));
}

// Converts adventurer data to json format
nlohmann::json Adventurer::toJson()
{
    Debug::dbg << "Converting adventurer " << name << " to JSON\n";
    nlohmann::json::object_t obj_values = {
        {"name", name},
        {"modifiers", modifiers},
        {"level", level},
        {"race", race},
        {"class", gameClass},
        {"hunger", hunger},
        {"strength", stats.strength},
        {"agility", stats.agility},
        {"fortitude", stats.fortitude},
        {"willpower", stats.willpower},
        {"perception", stats.perception},
        {"wisdom", stats.wisdom},
        {"magic", stats.magic},
        {"weaknesses", stats.weaknesses},
        {"strengths", stats.strengths}
    };
    return nlohmann::json(obj_values);
}

std::vector<std::string> Adventurer::toCharacterCard()
{
    Debug::dbg << "Creating character card for adventurer: " << name << "\n";
    std::vector<std::string> card = {
             std::string("/--------------------------\\"),
        gameUtil::fitStr("| Name: " + name, 27) + "|",
        gameUtil::fitStr("| Race: " + race, 27) + "|",
        gameUtil::fitStr("| Class: " + gameClass, 27) + "|",
        gameUtil::fitStr("| Level: " + std::to_string(level), 27) + "|",
        gameUtil::fitStr("| Stats: ", 27) + "|",
        gameUtil::fitStr("|  Strength: " + std::to_string(stats.strength), 27) + "|",
        gameUtil::fitStr("|  Agility: " + std::to_string(stats.agility), 27) + "|",
        gameUtil::fitStr("|  Fortitude: " + std::to_string(stats.fortitude), 27) + "|",
        gameUtil::fitStr("|  Willpower: " + std::to_string(stats.willpower), 27) + "|",
        gameUtil::fitStr("|  Perception: " + std::to_string(stats.perception), 27) + "|",
        gameUtil::fitStr("|  Wisdom: " + std::to_string(stats.wisdom), 27) + "|",
        gameUtil::fitStr("|  Magic: " + std::to_string(stats.magic), 27) + "|",
             std::string("\\--------------------------/")
    };
    return card;
}
