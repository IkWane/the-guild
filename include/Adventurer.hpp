#pragma once
#include <string>

class Adventurer
{
public:
    Adventurer(std::string name, int hunger)
        : name(name), hunger(hunger) {};
    ~Adventurer();
    std::string name;
    int hunger;
//    std::string favoredTerrains[];
//    std::string favoredEnemies[];
};
