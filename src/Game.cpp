#include "Game.hpp"

// initializes the game by loading a save file or creating a new game, based on player input
// loads configuration based on data/config.json
Game::Game()
{
    Debug::dbg << "Initializing window and RNG\n";
    RNG::get(std::time(nullptr));
    wm = WindowManager();

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

    Debug::dbg << "Starting new or loading existing game\n";
    giveDialog("Iintro");

    bool newGame = false;

    int choice = giveDialog("QhasSaveFile");

    switch (choice)
    {
        case 0:
        {
            bool invalidFile = true;
            while (invalidFile)
            {
                Debug::dbg << "Asking for save file name to load\n";
                saveFileName = "saves/" + textEntryDialog("EfileName") + ".json";
                Debug::dbg << "Checking if file exists: " << saveFileName << "\n";
                std::ifstream testFile(saveFileName.c_str());
                Debug::dbg << "File open status: " << testFile.fail() << "\n";
                invalidFile = testFile.fail();
                if (invalidFile)
                {
                    giveDialog("IfileSearchError");
                }
                else
                {
                    testFile.close();
                }
            }
            break;
        }
        case 1:
        {
            Debug::dbg << "Asking for save file name to create new game\n";
            saveFileName = "saves/" + textEntryDialog("EfileName") + ".json";
            newGame = true;
            break;
        }
        default:
            break;
    }

    if (!newGame)
    {
        try
        {
            guild = TheGuild(saveFileName.c_str());
        }
        catch (...)
        {
            giveDialog("IloadError");
            guild = TheGuild();
        }
    }
    else
    {
        guild = TheGuild(saveFileName);
    }
}

// starts the game loop
void Game::run()
{
    Debug::dbg << "Running game loop\n";

    guild.adventurers.push_back(newRandomAdventurer());
    guild.adventurers.push_back(newRandomAdventurer());
    guild.adventurers.push_back(newRandomAdventurer());
    guild.adventurers.push_back(newRandomAdventurer());
    
    Mission mission = newRandomMission(20);
    std::vector<std::string> advIdentifiers = guild.adventurerIdentifiers();
    mission.assignedAdventurers.insert(
        mission.assignedAdventurers.end(), 
        advIdentifiers.begin(), 
        advIdentifiers.end()
    );

    guild.addMission(mission);

    giveDialog("QRandomStuff", true);

    finishMission(mission);

    giveDialog("QRandomStuff", true);
    
    giveDialog("QendDemo");
    
    // wprintw(pad, "Current Guild gold: %d\n", guild.gold);
    // guild.gold += numberDialog("NdepositGold");
    // wprintw(pad, "Guild gold: %d\n", guild.gold);

    exitGame(true);
}

// sends a dialog to the player based on the data found at dialogName in the data/dialogs.json file
int Game::giveDialog(const char *dialogName, bool showHome, bool canQuit)
{
    Debug::dbg << "Giving dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];

    if (dialog.contains("text"))
    {
        wm.writeNewLine(dialog["text"].get<std::string>());
    }
    if (dialog.contains("choices"))
    {
        wm.writeNewLine();
        int option = -1;
        std::vector<std::string> options;
        for (int i = 0; i < dialog["choices"].size(); i++)
        {
            options.push_back(dialog["choices"][i].get<std::string>());
        }
        if (showHome)
        {
            options.push_back("Home");
        }
        
        while (option == -1)
        {
            option = gameUtil::chooseOption(
                wm,
                options, 
                dialog.contains("defaultChoice") ? dialog["defaultChoice"].get<int>() : 0, 
                true
            );
            if (option == -1)
            {
                if (canQuit)
                {
                    wm.removeLastLine();
                    int quitChoice = giveDialog("QquitGame", false, false);
                    if (quitChoice == 0)
                    {
                        exitGame(false);
                    } 
                    else if (quitChoice == 1)
                    {
                        exitGame();
                    }
                    else
                    {
                        wm.writeNewLine(dialog["text"].get<std::string>());
                        wm.writeNewLine();
                        option = -1;
                    }
                }
                
            }
            else if (option == options.size()-1 && showHome)
            {
                this->showHome();
                wm.writeNewLine(dialog["text"].get<std::string>());
                wm.writeNewLine();
                option = -1;
            }
        }
        Debug::dbg << "Player selected option " << option << " : " << dialog["choices"][option] << "\n";
        return option;
    }     
    return -1;
}

int Game::numberDialog(const char *dialogName)
{
    Debug::dbg << "Giving number dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];
    wm.writeNewLine(dialog["text"].get<std::string>());
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
        selected = gameUtil::chooseOption(wm, 7, list, selected);
        switch (selected)
        {
        case -1:
            {
                int quitChoice = giveDialog("QquitGame");
                if (quitChoice == 0)
                {
                    exitGame(false);
                }
                else if (quitChoice == 1)
                {
                    exitGame();
                }
                else
                {
                    wm.writeNewLine(dialog["text"].get<std::string>());
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
    wm.writeNewLine("> " + std::to_string(number));
    return number;
}

std::string Game::textEntryDialog(const char *dialogName)
{
    Debug::dbg << "Giving text entry dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];
    wm.writeNewLine(dialog["text"].get<std::string>());
    wm.writeNewLine();
    wm.updateWindow();
    std::string input = gameUtil::readStr(wm);
    while (input.empty())
    {
        wm.removeLastLine();
        int quitChoice = giveDialog("QquitGame");
        if (quitChoice == 0)
        {
            exitGame(false);
        } 
        else if (quitChoice == 1)
        {
            exitGame();
        }
        else
        {
            wm.writeNewLine();
            wm.writeNewLine(dialog["text"].get<std::string>());
            wm.writeNewLine();
            input = gameUtil::readStr(wm);
        }
    }
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
    throw GameExit(0);
}

Game::~Game()
{
    Debug::dbg << "Destroying game instance\n";
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
            int rand = rng.weightedInt(0, adventurer_modifiers.size(), adventurer_modifiers_weights);
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
        Debug::dbg << "Drawn modifier: " << adventurer_modifiers_keys[drawn] << "\n";
        adv.modifiers.push_back(adventurer_modifiers_keys[drawn]);
    }

    adv.race = races_keys[rng.weightedInt(0, races.size(), races_weights)];
    Debug::dbg << "Drawn race: " << adv.race << "\n";
    adv.gameClass = classes_keys[rng.weightedInt(0, classes.size(), classes_weights)];
    Debug::dbg << "Drawn class: " << adv.gameClass << "\n";

    updateAdventurerStatus(adv);
    adv.createIdentifier();
    
    return adv;
}

// updates the adventurer's stats based on his traits (race, class, modifiers)
void Game::updateAdventurerStatus(Adventurer &adv)
{
    Debug::dbg << "Updating adventurer status for: " << adv.name << "\n";
    adv.resetStats();
    updateAdventurerFromJsonKey(adv, adv.race, races);
    updateAdventurerFromJsonKey(adv, adv.gameClass, classes);
    Debug::dbg << adv.modifiers.size() << " modifiers to apply\n";
    for (auto &key : adv.modifiers)
    {
        updateAdventurerFromJsonKey(adv, key, adventurer_modifiers);
    }
    adv.balanceStats();
    adv.stats.balanceStrengthsAndWeaknesses();
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
        int actual_level = 0;
        std::map<std::string, int> mission_monsters = std::map<std::string, int>();
        std::vector<std::string> mission_monsters_keys = std::vector<std::string>();
        
        while (actual_level != level)
        {
            int rand = rng.weightedInt(0, monsters.size(), monsters_levels);
            if (actual_level + monsters_levels[rand] < level)
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
                actual_level += monsters_levels[rand];
            }
            else
            {
                int lowest_index = 0;
                int lowest_value = abs(actual_level + monsters_levels[0] - level);
                for (int i = 1; i < monsters_levels.size(); i++)
                {
                    int potential_lowest = abs(actual_level + monsters_levels[lowest_index] - level);
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
        int reward = actual_level * GOLD_ELIMINATION_MISSION_LEVEL_MUPLTIPLIER;
        Mission mission(std::string("Elimination quest"), actual_level, reward, mission_monsters);
        mission.terrainType = terrain_types_keys[rng.weightedInt(0, terrain_types.size(), terrain_types_weights)];
        mission.createIdentifier();
        return mission;
    }
    else
    {
        int reward = level * GOLD_TRAINING_MISSION_LEVEL_MUPLTIPLIER;
        Mission mission(std::string("Training quest"), level, reward, std::map<std::string, int>());
        mission.terrainType = terrain_types_keys[rng.weightedInt(0, terrain_types.size(), terrain_types_weights)];
        mission.createIdentifier();
        return mission;
    }
}

// calculates the total stats of all monsters in the mission
Stats Game::calculateMonstersStats(Mission &mission)
{
    Debug::dbg << "Calculating monster stats for mission\n";
    Stats monsterStats;
    for (const auto &[key, monsterCount] : mission.monsters)
    {
        nlohmann::json &monsterData = monsters[key];

        monsterStats.strength += (
            monsterData.contains("strength") ? 
            monsterData["strength"].get<int>() : 0
        ) * monsterCount;
        
        monsterStats.agility += (
            monsterData.contains("agility") ? 
            monsterData["agility"].get<int>() : 0
        ) * monsterCount;

        monsterStats.fortitude += (
            monsterData.contains("fortitude") ? 
            monsterData["fortitude"].get<int>() : 0
        ) * monsterCount;

        monsterStats.willpower += (
            monsterData.contains("willpower") ? 
            monsterData["willpower"].get<int>() : 0
        ) * monsterCount;

        monsterStats.perception += (
            monsterData.contains("perception") ? 
            monsterData["perception"].get<int>() : 0
        ) * monsterCount;

        monsterStats.wisdom += (
            monsterData.contains("wisdom") ? 
            monsterData["wisdom"].get<int>() : 0
        ) * monsterCount;

        monsterStats.magic += (
            monsterData.contains("magic") ? 
            monsterData["magic"].get<int>() : 0
        ) * monsterCount;
        if (monsterData[key].contains("strengths"))
        {
            for (int i = 0; i < monsterCount; i++)
            {
                monsterStats.strengths.insert(
                    monsterStats.strengths.end(),
                    monsterData["strengths"].begin(),
                    monsterData["strengths"].end()
                );
            }
            
        }
        if (monsterData[key].contains("weaknesses"))
        {
            for (int i = 0; i < monsterCount; i++)
            {
                monsterStats.weaknesses.insert(
                    monsterStats.weaknesses.end(),
                    monsterData["weaknesses"].begin(),
                    monsterData["weaknesses"].end()
                );
            }
        }
    }
    return monsterStats;
}

// calculates the total stats of all adventurers assigned to the mission
Stats Game::calculateTeamStats(Mission &mission)
{
    Debug::dbg << "Calculating team stats for mission\n";
    Stats teamStats;
    for (auto &adv : mission.assignedAdventurers)
    {
        std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(adv);
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
    teamStats.balanceStrengthsAndWeaknesses();
    return teamStats;
}

// determines if the mission is successful based on team stats, monster stats, and terrain type
MissionResult Game::determineSuccess(Mission &mission)
{
    Debug::dbg << "Determining mission success\n";
    Stats monsterStats = calculateMonstersStats(mission);
    Stats teamStats = calculateTeamStats(mission);
    int points = calculatePoints(teamStats, monsterStats, mission.terrainType);
    int probability = gameUtil::sigmoid(points, SUCCESS_STEEPNESS) * 100;
    Debug::dbg << "Mission success probability: " << probability << "\n";
    int roll = getDiceRoll(100);
    bool success = roll <= probability;
    Debug::dbg << "Mission roll " << roll << ": " << (success ? "Success" : "Failure")  << "\n";
    
    MissionResult result;
    result.success = success;
    result.lostAdventurers = determineSurvival(mission, monsterStats, success);
    result.gainedGold = (success ? mission.reward : 0) - LOST_ADVENTURER_COST * result.lostAdventurers.size();
    return result;
}

std::vector<std::string> Game::determineSurvival(Mission &mission, Stats &monsterStats, bool success)
{
    Debug::dbg << "Determining survival of adventurers";
    std::vector<std::string> lostAdv{};
    for (auto &advIdentifier : mission.assignedAdventurers)
    {
        Debug::dbg << "Determining survival of " << advIdentifier << "\n";
        std::optional<Adventurer *>adv = guild.getAdventurerByIdentifier(advIdentifier);
        if (adv.has_value())
        {
            int divisor = success ? mission.assignedAdventurers.size() * SUCCESS_ADV_TEAM_DIVISOR : FAILURE_ADV_DIVISOR;
            Debug::dbg << "Divisor: " << divisor << "\n";
            nlohmann::json terrainData = terrain_types[mission.terrainType];
            int points = 0;
            Debug::dbg << "Adventurer Stats vs Monster Stats\n";
            points += adv.value()->stats.strength - monsterStats.strength / divisor;
            points += adv.value()->stats.agility - monsterStats.agility / divisor;
            points += adv.value()->stats.fortitude - monsterStats.fortitude / divisor;
            points += adv.value()->stats.willpower - monsterStats.willpower / divisor;
            points += adv.value()->stats.perception * (terrainData.contains("visibility") ? terrainData["visibility"].get<int>() : 1) / 100 - monsterStats.perception / divisor;
            points += adv.value()->stats.wisdom - monsterStats.wisdom / divisor;
            points += adv.value()->stats.magic - monsterStats.magic / divisor;
            Debug::dbg << "Points after raw stats comparison: " << points << "\n";
            Debug::dbg << "Calculating strengths and weaknesses interactions\n";
            std::vector<std::string> monsterStrengths(monsterStats.strengths);
            std::vector<std::string> monsterWeaknessses(monsterStats.weaknesses);
            std::vector<std::string> advStrengths(adv.value()->stats.strengths);
            std::vector<std::string> advWeaknesses(adv.value()->stats.weaknesses);
            for (auto i = advStrengths.begin(); i != advStrengths.end(); ++i)
            {
                bool matched = false;
                for (auto j = monsterWeaknessses.begin(); j != monsterWeaknessses.end();++j)
                {
                    if (*i == *j)
                    {
                        points += MATCHING_TAG_POINTS;
                        matched = true;
                        advStrengths.erase(i);
                        monsterWeaknessses.erase(j);
                        break;
                    }
                }
                for (auto &tag : terrainData["tags"])
                {
                    if (*i == tag)
                    {
                        points += MATCHING_TAG_POINTS;
                    }
                }
                if (!matched)
                {
                    points += NONMATCHING_TAG_POINTS;
                }
                else
                {
                    break;
                }
                
            }
            for (auto i = advWeaknesses.begin(); i != advWeaknesses.end(); ++i)
            {
                bool matched = false;
                for (auto j = monsterStrengths.begin(); j != monsterStrengths.end();++j)
                {
                    if (*i == *j)
                    {
                        points -= MATCHING_TAG_POINTS;
                        matched = true;
                        advWeaknesses.erase(i);
                        monsterStrengths.erase(j);
                        break;
                    }
                }
                for (auto &tag : terrainData["tags"])
                {
                    if (*i == tag)
                    {
                        points -= MATCHING_TAG_POINTS;
                    }
                }
                if (!matched)
                {
                    points -= NONMATCHING_TAG_POINTS;
                }
                else
                {
                    break;
                }
            }
            Debug::dbg << "Adventurer calculated points: " << points << "\n";
            int probability = gameUtil::sigmoid(points, SUCCESS_STEEPNESS) * 100;
            Debug::dbg << "Adventurer survival probability: " << probability << "\n";
            int roll = getDiceRoll(100);
            bool success = roll <= probability;
            Debug::dbg << "Adventurer roll " << roll << ": " << (success ? "Success" : "Failure")  << "\n";
            if (!success)
            {
                lostAdv.push_back(advIdentifier);
            }
        }
        else
        {
            Debug::dbg << "Adventurer with name " << advIdentifier << "cannot be found, skipping\n";
        }
    }
    return lostAdv;
}

// calculates the points for the mission based on team stats, monster stats, and terrain type
int Game::calculatePoints(Stats &teamStats, Stats &monsterStats, std::string &terrainType)
{
    Debug::dbg << "Calculating points for mission\n";
    nlohmann::json terrainData = terrain_types[terrainType];
    int points = 0;
    Debug::dbg << "Team Stats vs Monster Stats\n";
    points += teamStats.strength - monsterStats.strength;
    points += teamStats.agility - monsterStats.agility;
    points += teamStats.fortitude - monsterStats.fortitude;
    points += teamStats.willpower - monsterStats.willpower;
    points += teamStats.perception * (terrainData.contains("visibility") ? terrainData["visibility"].get<int>() : 1) / 100 - monsterStats.perception;
    points += teamStats.wisdom - monsterStats.wisdom;
    points += teamStats.magic - monsterStats.magic;
    Debug::dbg << "Points after raw stats comparison: " << points << "\n";
    Debug::dbg << "Calculating strengths and weaknesses interactions\n";
    std::vector<std::string> monsterStrengths(monsterStats.strengths);
    std::vector<std::string> monsterWeaknessses(monsterStats.weaknesses);
    for (auto i = teamStats.strengths.begin(); i != teamStats.strengths.end(); ++i)
    {
        bool matched = false;
        for (auto j = monsterWeaknessses.begin(); j != monsterWeaknessses.end();++j)
        {
            if (*i == *j)
            {
                points += 5;
                matched = true;
                teamStats.strengths.erase(i);
                monsterWeaknessses.erase(j);
                break;
            }
        }
        for (auto &tag : terrainData["tags"])
        {
            if (*i == tag)
            {
                points += MATCHING_TAG_POINTS;
            }
        }
        if (!matched)
        {
            points += NONMATCHING_TAG_POINTS;
        }
        else
        {
            break;
        }
    }
    for (auto i = teamStats.weaknesses.begin(); i != teamStats.weaknesses.end(); ++i)
    {
        bool matched = false;
        for (auto j = monsterStrengths.begin(); j != monsterStrengths.end();++j)
        {
            if (*i == *j)
            {
                points -= MATCHING_TAG_POINTS;
                matched = true;
                teamStats.weaknesses.erase(i);
                monsterStrengths.erase(j);
                break;
            }
        }
        for (auto &tag : terrainData["tags"])
        {
            if (*i == tag)
            {
                points -= MATCHING_TAG_POINTS;
            }
        }
        if (!matched)
        {
            points -= NONMATCHING_TAG_POINTS;
        }
        else
        {
            break;
        }
    }
    Debug::dbg << "Final calculated points: " << points << "\n";
    return points;
}

void Game::finishMission(Mission &mission)
{
    MissionResult result = determineSuccess(mission);
    giveDialog("IMissionResult");
    bool lostAdv = result.lostAdventurers.size() > 0;
    std::string lostAdvLine = "";
    if (lostAdv)
    {
        for (int i = 0; i < result.lostAdventurers.size()-1; i++)
        {
            lostAdvLine += guild.getAdventurerByIdentifier(result.lostAdventurers[i]).value()->name + ", ";
            guild.removeAdventurerByIdentifier(result.lostAdventurers[i]);
        }
        lostAdvLine += guild.getAdventurerByIdentifier(result.lostAdventurers[result.lostAdventurers.size()-1]).value()->name;
        guild.removeAdventurerByIdentifier(result.lostAdventurers[result.lostAdventurers.size()-1]);
    }
    else
    {
        lostAdvLine = "None";
    }
    guild.removeMissionByIdentifier(mission);
    guild.gold += result.gainedGold;

    std::vector<std::string> card{
        std::string("Status: " + std::string(result.success ? "Success" : "Failure")),
        std::string("Lost adventurers: " + lostAdvLine),
        std::string("Gold gained: " + std::to_string(result.gainedGold))
    };
    gameUtil::renderCards(wm, {card});
}

// renders the character cards of the given adventurers side by side
void Game::renderCharacters(std::vector<std::string> adventurers)
{
    std::vector<std::vector<std::string>> cardsLines;
    for (auto &adv : adventurers)
    {
        std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(adv);
        if (advOpt.has_value())
        {
            cardsLines.push_back(advOpt.value()->toCharacterCard());
        }
    }
    gameUtil::renderCards(wm, cardsLines);
}

void Game::renderCharacters()
{
    std::vector<std::vector<std::string>> cardsLines;
    for (auto &adv : guild.adventurers)
    {
        cardsLines.push_back(adv.toCharacterCard());
    }
    gameUtil::renderCards(wm, cardsLines);
}

// renders the mission cards of the given missions side by side
void Game::renderMissions(std::vector<Mission> &missions)
{
    std::vector<std::vector<std::string>> cardsLines;
    for (auto &mission : missions)
    {
        cardsLines.push_back(toMissionCard(mission));
    }
    gameUtil::renderCards(wm, cardsLines);
}

void Game::showHome()
{
    int choice = giveDialog("QHome", false);
    switch (choice)
    {
    case 0:
        wm.writeNewLine("Current Balance : " + std::to_string(guild.gold));
        break;
    
    case 1:
        renderCharacters();
        break;
    
    case 2:
        renderMissions(guild.missions);
        break;
    
    default:
        break;
    }
}

// this method attempts to find and assign a random modifier to the adventurer that matches the
// specified benefit value. may fail to find a valid modifier within 10 tries, in which case no
// modifiers will be applied.
void Game::giveModWithBenefit(Adventurer &adv, int benefit)
{
    Debug::dbg << "Trying to give modifier with benefit " << benefit << " to adventurer: " << adv.name << "\n";
    RNG &rng = RNG::get();
    int drawn = -1;
    for (int i = 0; i < 10;i++)
    {
        int rand = rng.weightedInt(0, adventurer_modifiers.size(), adventurer_modifiers_weights);
        std::string key = adventurer_modifiers_keys[rand];
        if (adventurer_modifiers[key]["benefit"].get<int>() == benefit)
        {
            continue;
        }
        bool contains = false;
        for (int j = 0; j < adv.modifiers.size(); j++)
        {
            if (key == adv.modifiers[j])
            {
                contains = true;
                break;
            }
        }
        if (!contains)
        {
            drawn = rand;
            break;
        }
    }

    if (drawn != -1)
    {
        Debug::dbg << "Drew modifier: " << adventurer_modifiers_keys[drawn] << "\n";
        adv.modifiers.push_back(adventurer_modifiers_keys[drawn]);
        updateAdventurerStatus(adv);
    }
}

std::vector<std::string> toMissionCard(Mission &mission)
{
    return std::vector<std::string>();
}

// Creates a mission card (text-based) for the mission
// Output is a vector of strings, each string being a line
std::vector<std::string> Game::toMissionCard(Mission &mission)
{
    Debug::dbg << "Creating mission card for mission: " << mission.description << "\n";
    std::vector<std::string> card = {
        std::string("Mission: " + mission.description),
        std::string("Level: " + std::to_string(mission.level)),
        std::string("Reward: " + std::to_string(mission.reward)),
        std::string("Terrain: " + gameUtil::snakeToNormal(mission.terrainType, true)),
        std::string("Assigned Adventurers: "),
        
    };
    for (auto &adv : mission.assignedAdventurers)
    {
        std::optional<Adventurer*> advOptional = guild.getAdventurerByIdentifier(adv);
        if (advOptional.has_value())
        {
            card.push_back(" " + advOptional.value()->name);
        }
        else
        {
            Debug::dbg << "Adventurer doesn't exit !\n";
        }
    }
    card.push_back(std::string("Monsters:"));
    for (const auto &[key, value] : mission.monsters)
    {
        card.push_back(" " + gameUtil::snakeToNormal(key, true) + " x" + std::to_string(value));
    }
    return card;
}