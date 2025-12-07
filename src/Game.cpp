#include "Game.hpp"

Game::Game()
{
    RNG::get(0);
    window = initscr();

    cbreak();
    noecho();
    scrollok(window, TRUE);
    
    dialogs = FileManager::loadJson("data/dialogs.json");
    classes = FileManager::loadJson("data/characteristics/classes.json");
    adventurer_modifiers = FileManager::loadJson("data/characteristics/adventurer_modifiers.json");
    races = FileManager::loadJson("data/characteristics/races.json");
    terrain_types = FileManager::loadJson("data/characteristics/terrain_types.json");

    giveDialog("Iintro");

    bool invalidFile = true;
    bool triedStandardName = false;
    bool newGame = false;
    
    while (invalidFile)
    {
        int choice = giveDialog("QhasSaveFile");
        switch (choice)
        {
        case 0:
            if (triedStandardName)
            {
                saveFileName = textEntryDialog("EfileName");
                std::ifstream testFile(saveFileName.c_str());
                invalidFile = testFile.fail();
                if (invalidFile)
                {
                    giveDialog("IfileSearchError");
                }
            }
            else
            {
                saveFileName = "savefile.json";
                std::ifstream testFile(saveFileName.c_str());
                invalidFile = testFile.fail();
                if (invalidFile)
                {
                    giveDialog("InoStandardSaveFile");
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
                int quitChoice = giveDialog("QquitGame");
                if (quitChoice == 0)
                {
                    exitGame();
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
            giveDialog("IloadError");
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
    guild.gold += numberDialog("NdepositGold");
    printw("Guild gold: %d\n", guild.gold);
    getch();

    guild.saveGuild(saveFileName.c_str());
    return 0;
}

int Game::giveDialog(const char *dialogName)
{
    nlohmann::json dialog = dialogs[dialogName];

    if (dialog.contains("text"))
    {
        printw("%s\n", dialog["text"].get<std::string>().c_str());
    }
    if (dialog.contains("choices"))
    {
        if (dialog.contains("defaultChoice"))
        {
            return gameUtil::chooseOption(dialog["choices"], dialog["defaultChoice"].get<int>(), true);
        }
        
        return gameUtil::chooseOption(dialog["choices"], 0, true);
    }     
    return -1;
}

int Game::numberDialog(const char *dialogName)
{
    nlohmann::json dialog = dialogs[dialogName];
    printw("%s\n", dialog["text"].get<std::string>().c_str());
    std::string list[] = {"<<<", "<<", "<", "", ">", ">>", ">>>"};
    int selected = 0;
    int number = 0;
    while (selected != 3)
    {
        list[3] = std::to_string(number);
        selected = gameUtil::chooseOption(7, list, selected);
        switch (selected)
        {
        case -1:
            {
                int quitChoice = giveDialog("QquitGame");
                if (quitChoice == 0)
                {
                    exitGame();
                }
                else
                {
                    printw("%s\n", dialog["text"].get<std::string>().c_str());
                }
            }
            selected = 0;
            break;
        case 0:
            number -= 100;
            break;

        case 1:
            number -= 10;
            break;
        
        case 2:
            number -= 1;
            break;
        
        case 4:
            number += 1;
            break;
        
        case 5:
            number += 10;
            break;
        
        case 6:
            number += 100;
            break;
        
        default:
            break;
        }
    }
    printw("> %d\n", number);
    return number;
}

std::string Game::textEntryDialog(const char *dialogName)
{
    nlohmann::json dialog = dialogs[dialogName];
    printw("%s\n", dialog["text"].get<std::string>().c_str());
    std::string input = gameUtil::readStr();
    return input;
}

int Game::getDiceRoll(int max)
{
    return RNG::get().uniformInt(1, max);
}

void Game::exitGame()
{
    endwin();
    std::exit(0);
}

Game::~Game()
{
    endwin();
}
