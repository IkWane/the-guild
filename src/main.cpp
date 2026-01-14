#include <ncurses.h>
#include "Game.hpp"
#include "GameExit.hpp"

struct TestStruct {
    int i;
};

int main()
{
    std::string path = std::filesystem::current_path().string();
    std::string directory_name = path.substr(path.rfind('/'), path.length() - 1);
    if (directory_name == "/build")
    {
        std::filesystem::current_path(path.substr(0, path.rfind('/')));
    }
    
    std::filesystem::create_directories("logs/");
    try
    {
        Debug::initDebug("logs/game_log.txt");
        Debug::dbg << directory_name << "\n";
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