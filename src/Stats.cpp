#include "Stats.hpp"
#include "Debug.hpp"

// calculates the level appropriate for the stats
int Stats::calculateLevel()
{
    Debug::dbg << "Calculating level from stats\n";
    int points = (
        strength + 
        agility + 
        fortitude + 
        willpower + 
        perception + 
        wisdom + 
        magic
    ) + strengths.size() - weaknesses.size();
    Debug::dbg << "Total stat points: " << points << "\n";
    int level = int(sqrt(std::max(points, 1)));
    Debug::dbg << "Level: " << level << "\n";
    return level;
}

// removes strengths and weaknesses so they don't overlap
void Stats::balanceStrengthsAndWeaknesses()
{
    Debug::dbg << "Balancing strengths and weaknesses\n";
    std::vector<std::string> balancedStrengths;
    std::vector<std::string> balancedWeaknesses;
    for (auto &strength : strengths)
    {
        bool isBalanced = false;
        for (auto &weakness : weaknesses)
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
    for (auto &weakness : weaknesses)
    {
        bool isBalanced = false;
        for (auto &strength : strengths)
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
    strengths = balancedStrengths;
    weaknesses = balancedWeaknesses;
}
