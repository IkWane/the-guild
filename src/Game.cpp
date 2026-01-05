#include "Game.hpp"

// initializes the game by loading a save file or creating a new game, based on player input
// loads configuration based on data/config.json
Game::Game()
{
    Debug::dbg << "Initializing window and RNG\n";
    RNG::get(0);
    window = initscr();

    cbreak();
    noecho();
    scrollok(window, TRUE);

    nlohmann::json config = FileManager::loadJson("data/config.json");

    dialogs = FileManager::loadJson(config["dialogs_file"].get<std::string>().c_str());
    adventurer_names = FileManager::loadJson(config["adventurer_names_file"].get<std::string>().c_str());
    gameUtil::loadJsonConfig(
        config["classes_file"].get<std::string>(),
        classes, 
        classes_keys, 
        classes_weights
    );
    gameUtil::loadJsonConfig(
        config["adventurer_modifiers_file"].get<std::string>(), 
        adventurer_modifiers, 
        adventurer_modifiers_keys, 
        adventurer_modifiers_weights
    );
    gameUtil::loadJsonConfig(
        config["races_file"].get<std::string>(), 
        races, 
        races_keys, 
        races_weights
    );
    gameUtil::loadJsonConfig(
        config["terrain_types_file"].get<std::string>(), 
        terrain_types, 
        terrain_types_keys, 
        terrain_types_weights
    );
    gameUtil::loadJsonConfig(
        config["monsters_file"].get<std::string>(), 
        monsters, 
        monsters_keys, 
        monsters_levels
    );
    
    

    getch();

    Debug::dbg << "Starting new or loading existing game\n";
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
    Debug::dbg << "Giving dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];

    if (dialog.contains("text"))
    {
        printw("%s\n", dialog["text"].get<std::string>().c_str());
    }
    if (dialog.contains("choices"))
    {
        int option = -1;
        while (option == -1)
        {
            option = gameUtil::chooseOption(
                dialog["choices"], 
                dialog.contains("defaultChoice") ? dialog["defaultChoice"].get<int>() : 0, 
                true
            );
            if (option == -1)
            {
                int quitChoice = giveDialog("QquitGame");
                if (quitChoice == 0)
                {
                    exitGame();
                }
                else
                {
                    printw("%s\n", dialog["text"].get<std::string>().c_str());
                    option = -1;
                }
            }
        }
        return option;
    }     
    return -1;
}

int Game::numberDialog(const char *dialogName)
{
    Debug::dbg << "Giving number dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];
    printw("%s\n", dialog["text"].get<std::string>().c_str());
    int min, max;
    if (dialog.contains("min"))
    {
        min = dialog["min"].get<int>();
    }
    else
    {
        min = -2147483648;
    }
    if (dialog.contains("max"))
    {
        max = dialog["max"].get<int>();
    }
    else
    {
        max = 2147483647;
    }
    
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
                    printw("\n%s\n", dialog["text"].get<std::string>().c_str());
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
    number = std::clamp(number, min, max);
    printw("> %d\n", number);
    return number;
}

std::string Game::textEntryDialog(const char *dialogName)
{
    Debug::dbg << "Giving text entry dialog: " << dialogName << "\n";
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
    Debug::dbg << "Exiting game\n";
    if (save)
    {
        guild.saveGuild(saveFileName.c_str());
    }
    endwin();
    throw GameExit(0);
}

Game::~Game()
{
    Debug::dbg << "Destroying game instance\n";
    endwin();
}

// creates a new adventurer with randomized traits
Adventurer Game::newRandomAdventurer()
{
    Debug::dbg << "Creating new random adventurer\n";
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
    int modifierCount = rng.uniformInt(1, 6);
    int drawn_indices[] = {-1, -1, -1, -1, -1, -1};

    for (int i = 0; i < modifierCount; i++)
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

    adv.race = races_keys[rng.weightedInt(0, races.size()-1, races_weights)];
    adv.gameClass = classes_keys[rng.weightedInt(0, classes.size()-1, classes_weights)];
    
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
    Debug::dbg << "Updating adventurer " << adv.name << " from key: " << key << "\n";
    nlohmann::json &data = dataFrom[key];
    if (data.contains("strength"))
    {
        adv.stats.strength += data["strength"].get<int>();
    }
    if (data.contains("agility"))
    {
        adv.stats.agility += data["agility"].get<int>();
    }
    if (data.contains("fortitude"))
    {
        adv.stats.fortitude += data["fortitude"].get<int>();
    }
    if (data.contains("willpower"))
    {
        adv.stats.willpower += data["willpower"].get<int>();
    }
    if (data.contains("perception"))
    {
        adv.stats.perception += data["perception"].get<int>();
    }
    if (data.contains("wisdom"))
    {
        adv.stats.wisdom += data["wisdom"].get<int>();
    }
    if (data.contains("magic"))
    {
        adv.stats.magic += data["magic"].get<int>();
    }
    if (data.contains("strengths"))
    {
        for (auto &el : data["strengths"])
        {
            adv.stats.strengths.push_back(el.get<std::string>());
        }
    }
    if (data.contains("weaknesses"))
    {
        for (auto &el : data["weaknesses"])
        {
            adv.stats.weaknesses.push_back(el.get<std::string>());
        }
    }
}

// Creates a mission with randomized parameter, found within the json config files
Mission Game::newRandomMission(int level)
{
    Debug::dbg << "Creating new random mission of level: " << level << "\n";
    RNG &rng = RNG::get();
    if (level >= 5)
    {
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
        Mission mission(std::string("Elimination quest"), current_level, mission_monsters, mission_monsters_keys);
        mission.terrainType = terrain_types_keys[rng.weightedInt(0, terrain_types.size() - 1, terrain_types_weights)];
    }
    else
    {
        Mission mission(std::string("Training quest"), level, std::map<std::string, int>(), std::vector<std::string>());
        mission.terrainType = terrain_types_keys[rng.weightedInt(0, terrain_types.size() - 1, terrain_types_weights)];
        return mission;
    }
}

Stats Game::calculateMissionStats(Mission &mission)
{
    Stats monsterStats;
    for (auto &key : mission.monsters_keys)
    {
        nlohmann::json &monsterData = monsters[key];

        monsterStats.strength += (
            monsterData.contains("strength") ? 
            monsterData["strength"].get<int>() : 0
        ) * mission.monsters[key];
        
        monsterStats.agility += (
            monsterData.contains("agility") ? 
            monsterData["agility"].get<int>() : 0
        ) * mission.monsters[key];

        monsterStats.fortitude += (
            monsterData.contains("fortitude") ? 
            monsterData["fortitude"].get<int>() : 0
        ) * mission.monsters[key];

        monsterStats.willpower += (
            monsterData.contains("willpower") ? 
            monsterData["willpower"].get<int>() : 0
        ) * mission.monsters[key];

        monsterStats.perception += (
            monsterData.contains("perception") ? 
            monsterData["perception"].get<int>() : 0
        ) * mission.monsters[key];

        monsterStats.wisdom += (
            monsterData.contains("wisdom") ? 
            monsterData["wisdom"].get<int>() : 0
        ) * mission.monsters[key];

        monsterStats.magic += (
            monsterData.contains("magic") ? 
            monsterData["magic"].get<int>() : 0
        ) * mission.monsters[key];
        if (monsterData[key].contains("strengths"))
        {
            monsterStats.strengths.insert(
                monsterStats.strengths.end(),
                monsterData["strengths"].begin(),
                monsterData["strengths"].end()
            );
        }
        if (monsterData[key].contains("weaknesses"))
        {
            monsterStats.weaknesses.insert(
                monsterStats.weaknesses.end(),
                monsterData["weaknesses"].begin(),
                monsterData["weaknesses"].end()
            );
        }
    }

    return monsterStats;
}

Stats Game::calculateTeamStats(Mission &mission)
{
    Stats teamStats;
    for (auto &adv : mission.assignedAdventurers)
    {
        std::optional<Adventurer*> advOpt = guild.getAdventurerByName(adv);
        if (advOpt.has_value())
        {
            Adventurer *advPtr = advOpt.value();
            teamStats.strength += advPtr->stats.strength;
            teamStats.agility += advPtr->stats.agility;
            teamStats.fortitude += advPtr->stats.fortitude;
            teamStats.willpower += advPtr->stats.willpower;
            teamStats.perception += advPtr->stats.perception;
            teamStats.wisdom += advPtr->stats.wisdom;
            teamStats.magic += advPtr->stats.magic;
            teamStats.strengths.insert(
                teamStats.strengths.end(),
                advPtr->stats.strengths.begin(),
                advPtr->stats.strengths.end()
            );
            teamStats.weaknesses.insert(
                teamStats.weaknesses.end(),
                advPtr->stats.weaknesses.begin(),
                advPtr->stats.weaknesses.end()
            );
        }
    }
    return teamStats;
}

bool Game::determineSuccess(Mission &mission)
{
    Debug::dbg << "Determining mission success\n";
    Stats monsterStats = calculateMissionStats(mission);
    Stats teamStats = calculateTeamStats(mission);
    int points = calculatePoints(teamStats, monsterStats, mission.terrainType);
    Debug::dbg << "Mission points calculated: " << points << "\n";
    int probability = gameUtil::sigmoid(points, 0.1);
    Debug::dbg << "Mission success probability: " << probability << "\n";
    int roll = getDiceRoll(100);
    Debug::dbg << "Mission roll: " << roll << "\n";
    if (roll >= probability)
    {
        return true;
    }
    return false;
}

int Game::calculatePoints(Stats &teamStats, Stats &monsterStats, std::string &terrainType)
{
    nlohmann::json terrainData = terrain_types[terrainType];
    int points = 0;
    points += teamStats.strength - monsterStats.strength;
    points += teamStats.agility - monsterStats.agility;
    points += teamStats.fortitude - monsterStats.fortitude;
    points += teamStats.willpower - monsterStats.willpower;
    points += teamStats.perception / (terrainData.contains("visibility") ? terrainData["visibility"].get<int>() : 1) - monsterStats.perception;
    points += teamStats.wisdom - monsterStats.wisdom;
    points += teamStats.magic - monsterStats.magic;
    for (auto &strength : teamStats.strengths)
    {
        for (auto &weakness : monsterStats.weaknesses)
        {
            if (strength == weakness)
            {
                points += 5;
            }
            else 
            {
                points += 1;
            }
        }
        for (auto &tag : terrainData["tags"])
        {
            if (strength == tag)
            {
                points += 5;
            }
        }
    }
    for (auto &weakness : teamStats.weaknesses)
    {
        for (auto &strength : monsterStats.strengths)
        {
            if (weakness == strength)
            {
                points -= 5;
            }
            else 
            {
                points -= 1;
            }
        }
        for (auto &tag : terrainData["tags"])
        {
            if (weakness == tag)
            {
                points -= 5;
            }
        }
    }
    return points;
}