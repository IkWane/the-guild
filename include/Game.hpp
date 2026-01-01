#pragma once
#include <iostream>
#include <string>
#include <math.h>
#include <ncurses.h>
#include "gameUtil.hpp"
#include "FileManager.hpp"
#include "TheGuild.hpp"

class Game {
public:
    Game();
    int run();
    int giveDialog(const char *dialogName);
    int numberDialog(const char *dialogName);
    std::string textEntryDialog(const char *dialogName);
    int getDiceRoll(int max);
    void exitGame(bool save = true);
    ~Game();
    Adventurer newRandomAdventurer();
    void updateAdventurerStatus(Adventurer &adv);
    void updateAdventurerFromJsonKey(Adventurer &adv, std::string &key, nlohmann::json &extractFrom);
    Mission newRandomMission(int level);
private:
    WINDOW *window;
    nlohmann::json dialogs;
    nlohmann::json adventurer_names;
    nlohmann::json adventurer_modifiers;
    std::vector<std::string> adventurer_modifiers_keys;
    std::vector<int> adventurer_modifiers_weights;
    nlohmann::json races;
    nlohmann::json classes;
    std::vector<std::string> classes_keys;
    std::vector<int> classes_weights;
    nlohmann::json terrain_types;
    nlohmann::json monsters;
    std::vector<std::string> monsters_keys;
    std::vector<int> monsters_levels;
    TheGuild guild;
    std::string saveFileName;

};