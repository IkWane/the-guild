#include <string>

class Adventurer
{
public:
    Adventurer(std::string name, int healthPoints)
        : name(name), healthPoints(healthPoints) {};
    std::string name;
    int healthPoints;
};