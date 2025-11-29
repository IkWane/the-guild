#include "Game.hpp"

Game::Game()
{
    RNG::get(0);
    window = initscr();

    cbreak();
    noecho();
    scrollok(window, TRUE);
    
    dialogs = FileManager::loadJson("data/dialogs.json");

    gameUtil::giveDialog(dialogs["Iintro"]);

    bool invalidFile = true;
    bool triedStandardName = false;
    bool newGame = false;
    
    while (invalidFile)
    {
        int choice = gameUtil::giveDialog(dialogs["QhasSaveFile"]);
        switch (choice)
        {
        case 0:
            if (triedStandardName)
            {
                saveFileName = gameUtil::textEntryDialog(dialogs["EfileName"]);
                std::ifstream testFile(saveFileName.c_str());
                invalidFile = testFile.fail();
                if (invalidFile)
                {
                    gameUtil::giveDialog(dialogs["IfileSearchError"]);
                }
            }
            else
            {
                saveFileName = "savefile.json";
                std::ifstream testFile(saveFileName.c_str());
                invalidFile = testFile.fail();
                if (invalidFile)
                {
                    gameUtil::giveDialog(dialogs["InoStandardSaveFile"]);
                }
                triedStandardName = true;
            }
            break;
        case 1:
            saveFileName = "savefile.json";
            invalidFile = false;
            newGame = true;
            break;
        case -1:
            {
                int quitChoice = gameUtil::giveDialog(dialogs["QquitGame"]);
                if (quitChoice == 0)
                {
                    endwin();
                    std::exit(0);
                }
            }
            break;
        default:
            break;
        }
    }

    if (!newGame)
    {
        try
        {
            guild = TheGuild(saveFileName);
        }
        catch (...)
        {
            gameUtil::giveDialog(dialogs["IloadError"]);
            guild = TheGuild();
        }
    }
    else
    {
        guild = TheGuild();
    }
}

int Game::run()
{
    printw("Current Guild gold: %d\n", guild.gold);
    guild.gold += gameUtil::numberDialog(dialogs["NdepositGold"]);
    printw("Guild gold: %d\n", guild.gold);
    getch();

    guild.saveGuild(saveFileName.c_str());
    return 0;
}

Game::~Game()
{
    endwin();
}
