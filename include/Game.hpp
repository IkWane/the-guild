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
    ~Game();
private:
    WINDOW *window;
    nlohmann::json dialogs;
    nlohmann::json adventurer_modifiers;
    nlohmann::json races;
    nlohmann::json classes;
    nlohmann::json terrain_types;
    TheGuild guild;
    std::string saveFileName;

};