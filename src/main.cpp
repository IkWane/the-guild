// #include <iostream>
// #include <string>
// #include <math.h>
// #include "gameUtil.hpp"
// #include "FileManager.hpp"
// #include "TheGuild.hpp"
#include <ncurses.h>
#include "Game.hpp"
#include "Debug.hpp"

int main()
{
    Debug::initDebug("test.txt");
    Debug::dbg << "tee";
    Game game = Game();
    game.run();

    return 0;
}