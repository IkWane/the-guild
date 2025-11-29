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
    TheGuild guild;
    std::string saveFileName;

};