#include <ncurses.h>
#include "Game.hpp"
#include <algorithm>

struct TestStruct {
    int i;
};

int main()
{
    try
    {
        Debug::initDebug("logs/game_log.txt");
        Debug::dbg << std::string("\\").length() << "\n";
        Game game = Game();
        game.run();
    }
    catch(const GameExit& e)
    {
        if (!e.message.empty())
        {
            std::cerr << e.message;
            return 1;
        }
    }
    return 0;
}