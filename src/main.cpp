#include <ncurses.h>
#include "Game.hpp"
#include "Debug.hpp"
#include "GameExit.hpp"
#include "WindowManager.hpp"

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
        return e.value;
    }
    // Debug::initDebug("logs/game_log.txt");
    // WindowManager wm = WindowManager();
    // wm.writeNewLine("Hello, World!");
    // wm.waitForKeyPress();
    // wm.writeNewLine("This is a test of the WindowManager.");
    // wm.waitForKeyPress();
    // int ch = 0;
    // while (ch != DEFAULT_KEY_ESCAPE)
    // {
    //     ch = wm.waitForKeyPress();
    //     wm.writeNewLine(std::to_string(ch));
    //     wm.updateWindow();
    // }
    
    // wm.waitForKeyPress();
    return 0;
}