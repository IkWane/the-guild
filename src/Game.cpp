#include "Game.hpp"

// initializes the game by loading a save file or creating a new game, based on player input
// loads configuration based on data/config.json
Game::Game()
{
    RNG::get(0);
    window = initscr();

    cbreak();
    noecho();
    scrollok(window, TRUE);
    
    nlohmann::json config = FileManager::loadJson("data/config.json");

    dialogs = FileManager::loadJson(config["dialogs_file"].get<std::string>().c_str());
    classes = FileManager::loadJson(config["classes_file"].get<std::string>().c_str());
    classes_weights = std::vector<int>();
    classes_keys = std::vector<std::string>();
    for (auto& [key, val] : classes.items())
    {
        classes_weights.push_back(val["weight"].get<int>());
        classes_keys.push_back(key);
    }
    adventurer_modifiers = FileManager::loadJson(config["adventurer_modifiers_file"].get<std::string>().c_str());
    adventurer_modifiers_weights = std::vector<int>();
    adventurer_modifiers_keys = std::vector<std::string>();
    for (auto& [key, val] : adventurer_modifiers.items())
    {
        adventurer_modifiers_weights.push_back(val["weight"].get<int>());
        adventurer_modifiers_keys.push_back(key);
    }
    adventurer_names = FileManager::loadJson(config["adventurer_names_file"].get<std::string>().c_str());
    races = FileManager::loadJson(config["races_file"].get<std::string>().c_str());
    terrain_types = FileManager::loadJson(config["terrain_types_file"].get<std::string>().c_str());
    monsters = FileManager::loadJson(config["monsters_file"].get<std::string>().c_str());
    monsters_levels = std::vector<int>();
    monsters_keys = std::vector<std::string>();
    for (auto& [key, val] : monsters.items())
    {
        monsters_levels.push_back(val["level"].get<int>());
        monsters_keys.push_back(key);
    }

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
                    exitGame(false);
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

// starts the game loop
int Game::run()
{
    printw("Current Guild gold: %d\n", guild.gold);
    guild.gold += numberDialog("NdepositGold");
    printw("Guild gold: %d\n", guild.gold);
    getch();

    guild.saveGuild(saveFileName.c_str());
    return 0;
}

// sends a dialog to the player based on the data found at dialogName in the data/dialogs.json file
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

// gets a random number between 1 and max (included)
int Game::getDiceRoll(int max)
{
    return RNG::get().uniformInt(1, max);
}

// closes the program, and saves if optional parameter is set to true
void Game::exitGame(bool save)
{
    if (save)
    {
        guild.saveGuild(saveFileName.c_str());
    }
    endwin();
    std::exit(0);
}

Game::~Game()
{
    endwin();
}

// creates a new adventurer with randomized traits
Adventurer Game::newRandomAdventurer()
{
    RNG &rng = RNG::get();
    std::string advFirstName = adventurer_names["first_names"][rng.uniformInt(
        0, 
        adventurer_names["first_names"].size() - 1
    )].get<std::string>();

    std::string advLastName = adventurer_names["last_names"][rng.uniformInt(
        0, 
        adventurer_names["last_names"].size() - 1
    )].get<std::string>();

    Adventurer adv = Adventurer(advFirstName + " " + advLastName, 100);
    int drawn_indices[] = {-1, -1, -1, -1, -1};

    for (int i = 0; i < 5; i++)
    {
        int drawn = -1;
        while (drawn == -1) {
            int rand = rng.weightedInt(0, adventurer_modifiers.size() - 1, adventurer_modifiers_weights);
            bool contains = false;
            for (int j = 0; j < i+1; j++)
            {
                if (drawn_indices[j] == rand)
                {
                    contains = true;
                    break;
                }
            }

            if (!contains)
            {
                drawn = rand;
            }
        }
        drawn_indices[i] = drawn;
        adv.modifiers.push_back(adventurer_modifiers_keys[drawn]);
    }

    updateAdventurerStatus(adv);
    
    return adv;
}

// updates the adventurer's stats based on his traits (race, class, modifiers)
void Game::updateAdventurerStatus(Adventurer &adv)
{
    adv.resetStats();
    updateAdventurerFromJsonKey(adv, adv.race, races);
    updateAdventurerFromJsonKey(adv, adv.gameClass, classes);
    for (auto &key : adv.modifiers)
    {
        updateAdventurerFromJsonKey(adv, key, adventurer_modifiers);
    }
    adv.updateLevel();
}

// updates the adventurer's stats based on data from the json found at key in dataFrom
void Game::updateAdventurerFromJsonKey(Adventurer &adv, std::string &key, nlohmann::json &dataFrom)
{
    nlohmann::json &data = dataFrom[key];
    if (data.contains("strength"))
    {
        adv.strength += data["strength"].get<int>();
    }
    if (data.contains("agility"))
    {
        adv.agility += data["agility"].get<int>();
    }
    if (data.contains("fortitude"))
    {
        adv.fortitude += data["fortitude"].get<int>();
    }
    if (data.contains("willpower"))
    {
        adv.willpower += data["willpower"].get<int>();
    }
    if (data.contains("perception"))
    {
        adv.perception += data["perception"].get<int>();
    }
    if (data.contains("wisdom"))
    {
        adv.wisdom += data["wisdom"].get<int>();
    }
    if (data.contains("magic"))
    {
        adv.magic += data["magic"].get<int>();
    }
    if (data.contains("strengths"))
    {
        for (auto &el : data["strengths"])
        {
            adv.strengths.push_back(el.get<std::string>());
        }
    }
    if (data.contains("weaknesses"))
    {
        for (auto &el : data["weaknesses"])
        {
            adv.weaknesses.push_back(el.get<std::string>());
        }
    }
}

Mission Game::newRandomMission(int level)
{
    RNG &rng = RNG::get();
    int current_level = 0;
    std::map<std::string, int> mission_monsters = std::map<std::string, int>();
    std::vector<std::string> mission_monsters_keys = std::vector<std::string>();
    
    while (current_level != level)
    {
        int rand = rng.weightedInt(0, monsters.size() - 1, monsters_levels);
        if (current_level + monsters_levels[rand] < level)
        {
            std::string &key = monsters_keys[rand];
            if (mission_monsters.contains(key))
            {
                mission_monsters[key] += 1;
            }
            else
            {
                mission_monsters_keys.push_back(key);
                mission_monsters[key] = 1;
            }
            current_level += monsters_levels[rand];
        }
        else
        {
            int lowest_index = 0;
            int lowest_value = abs(current_level + monsters_levels[0] - level);
            for (int i = 1; i < monsters_levels.size(); i++)
            {
                int potential_lowest = abs(current_level + monsters_levels[lowest_index] - level);
                if (potential_lowest < lowest_value)
                {
                    lowest_value = potential_lowest;
                    lowest_index = i;
                }
            }
            std::string &key = monsters_keys[lowest_index];
            if (mission_monsters.contains(key))
            {
                mission_monsters[key] += 1;
            }
            else
            {
                mission_monsters_keys.push_back(key);
                mission_monsters[key] = 1;
            }
            break;
        }
        
    }
    

    return Mission();
}
