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
#include "MissionResult.hpp"
#include <climits>

class Game {
public:
    Game();
    void run();
    int giveDialog(const char *dialogName, bool _showHome = false, bool canQuit = true);
    int numberDialog(const char *dialogName, bool _showHome = false);
    std::string textEntryDialog(const char *dialogName);
    std::vector<std::string> adventurerPtrSelectionDialog(std::vector<Adventurer*> &adventurers, int max = -1);
    std::vector<std::string> adventurerSelectionDialog(std::vector<Adventurer> &adventurers, int max = -1);
    void missionSelectionDialog(std::vector<Mission> &missions);
    int getDiceRoll(int max);
    void exitGame(bool save = true);
    ~Game();
    Adventurer newRandomAdventurer();
    void updateAdventurerStatus(Adventurer &adv);
    void updateAdventurerFromJsonKey(Adventurer &adv, std::string &key, nlohmann::json &extractFrom);
    Mission newRandomMission(int level);
    Stats calculateMonstersStats(Mission &mission);
    Stats calculateTeamStats(Mission &mission);
    MissionResult determineSuccess(Mission &mission);
    std::vector<std::string> determineSurvival(Mission &mission, Stats &monsterStats, bool success);
    int calculatePoints(Stats &teamStats, Stats &monsterStats, std::string &terrainType);
    void finishMission(Mission &mission);
    void renderAdventurers(std::vector<std::string> adventurers);
    void renderAdventurers(std::vector<Adventurer> &adventurers);
    void renderAdventurers();
    void renderMissions(std::vector<Mission> &missions);
    void showHome();
    std::string giveModWithBenefit(Adventurer &adv, int benefit);
    std::pair<std::string, int> giveAdditionalStat(Adventurer &adv);
    std::vector<std::string> toMissionCard(Mission &mission);
    void advanceDay();
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
    std::vector<int> monsters_weights;
    TheGuild guild;
    std::string saveFileName;
};