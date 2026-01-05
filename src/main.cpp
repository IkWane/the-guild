#include <ncurses.h>
#include "Game.hpp"
#include "Debug.hpp"
#include "GameExit.hpp"

int main()
{
    try
    {
        Debug::initDebug("log.txt");
        Game game = Game();
        game.run();
    }
    catch(const GameExit& e)
    {
        return e.value;
    }
    return 0;
}