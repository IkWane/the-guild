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
private:
    WINDOW *window;
    nlohmann::json dialogs;
    nlohmann::json adventurer_modifiers;
    nlohmann::json races;
    nlohmann::json classes;
    nlohmann::json terrain_types;
    nlohmann::json monsters;
    TheGuild guild;
    std::string saveFileName;

};