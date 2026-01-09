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

// Updates the adventurer's level based on their stats
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

// makes sure stats are within acceptable bounds
void Adventurer::balanceStats()
{
    Debug::dbg << "Balancing stats for adventurer: " << name << "\n";
    stats.strength = std::max(1, stats.strength); // normal stats must be at least 1 -> basic abilities
    stats.agility = std::max(1, stats.agility);
    stats.fortitude = std::max(1, stats.fortitude);
    stats.willpower = std::max(1, stats.willpower);
    stats.perception = std::max(1, stats.perception);
    stats.wisdom = std::max(1, stats.wisdom);
    stats.magic = std::max(0, stats.magic); // magic can be 0 -> no magic ability
}

// removes strength and weaknesses so they don't overlap
void Adventurer::balanceStrengthsAndWeaknesses()
{
    Debug::dbg << "Balancing strengths and weaknesses for adventurer: " << name << "\n";
    std::vector<std::string> balancedStrengths;
    std::vector<std::string> balancedWeaknesses;
    for (auto &strength : stats.strengths)
    {
        bool isBalanced = false;
        for (auto &weakness : stats.weaknesses)
        {
            if (strength == weakness)
            {
                isBalanced = true;
                break;
            }
        }
        if (!isBalanced)
        {
            balancedStrengths.push_back(strength);
        }
    }
    for (auto &weakness : stats.weaknesses)
    {
        bool isBalanced = false;
        for (auto &strength : stats.strengths)
        {
            if (weakness == strength)
            {
                isBalanced = true;
                break;
            }
        }
        if (!isBalanced)
        {
            balancedWeaknesses.push_back(weakness);
        }
    }
    stats.strengths = balancedStrengths;
    stats.weaknesses = balancedWeaknesses;
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

// Creates a character card (text-based) for the adventurer
// Output is a vector of strings, each string being a line
std::vector<std::string> Adventurer::toCharacterCard()
{
    Debug::dbg << "Creating character card for adventurer: " << name << "\n";
    //  gameUtil::fitStr("/", len-1, '-') + "\\",
    std::vector<std::string> card = {
        std::string("| Name: " + name),
        std::string("| Race: " + race),
        std::string("| Class: " + gameClass),
        std::string("| Level: " + std::to_string(level)),
        std::string("| Stats: "),
        std::string("|  Strength: " + std::to_string(stats.strength)),
        std::string("|  Agility: " + std::to_string(stats.agility)),
        std::string("|  Fortitude: " + std::to_string(stats.fortitude)),
        std::string("|  Willpower: " + std::to_string(stats.willpower)),
        std::string("|  Perception: " + std::to_string(stats.perception)),
        std::string("|  Wisdom: " + std::to_string(stats.wisdom)),
        std::string("|  Magic: " + std::to_string(stats.magic)),
        std::string("| Modifiers: ")
    };
    for (auto &mod : modifiers)
    {
        card.push_back(std::string("|  " + gameUtil::snakeToNormal(mod, true)));
    }
    int len = 0;
    for (auto &line : card)
    {
        if (len < int(line.length()))
        {
            len = int(line.length());
        }
    }
    len += 2; // padding
    for (auto &line : card)
    {
        line = gameUtil::fitStr(line, len-1) + "|";
    }
    
    card.insert(card.begin(), gameUtil::fitStr("/", len-1, '-') + "\\");
    card.push_back(gameUtil::fitStr("\\", len-1, '-') + "/");
    return card;
}
