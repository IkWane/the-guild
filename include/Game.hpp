#pragma once
#include <iostream>
#include <string>
#include <math.h>
#include <ncurses.h>
#include "gameUtil.hpp"
#include "FileManager.hpp"
#include "TheGuild.hpp"
#include "Debug.hpp"
#include <algorithm>
#include "GameExit.hpp"
#include <ctime>
#include "WindowManager.hpp"

class Game {
public:
    Game();
    void run();
    int giveDialog(const char *dialogName, bool showHome = false, bool canQuit = true);
    int numberDialog(const char *dialogName);
    std::string textEntryDialog(const char *dialogName);
    int getDiceRoll(int max);
    void exitGame(bool save = true);
    ~Game();
    Adventurer newRandomAdventurer();
    void updateAdventurerStatus(Adventurer &adv);
    void updateAdventurerFromJsonKey(Adventurer &adv, std::string &key, nlohmann::json &extractFrom);
    Mission newRandomMission(int level);
    Stats calculateMonstersStats(Mission &mission);
    Stats calculateTeamStats(Mission &mission);
    bool determineSuccess(Mission &mission);
    int calculatePoints(Stats &teamStats, Stats &monsterStats, std::string &terrainType);
    void renderCharacters(std::vector<std::string> adventurers);
    void renderCharacters();
    void renderMissions(std::vector<Mission> &missions);
    void showHome();
private:
    WindowManager wm;
    nlohmann::json dialogs;
    nlohmann::json adventurer_names;
    nlohmann::json adventurer_modifiers;
    std::vector<std::string> adventurer_modifiers_keys;
    std::vector<int> adventurer_modifiers_weights;
    nlohmann::json races;
    std::vector<std::string> races_keys;
    std::vector<int> races_weights;
    nlohmann::json classes;
    std::vector<std::string> classes_keys;
    std::vector<int> classes_weights;
    nlohmann::json terrain_types;
    std::vector<std::string> terrain_types_keys;
    std::vector<int> terrain_types_weights;
    nlohmann::json monsters;
    std::vector<std::string> monsters_keys;
    std::vector<int> monsters_levels;
    TheGuild guild;
    std::string saveFileName;
};