#pragma once
#include <vector>
#include <string>

class MissionResult {
public:
    std::vector<std::string> lostAdventurers;
    int gainedGold;
    bool success;
};