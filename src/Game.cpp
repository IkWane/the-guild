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
        monsters_weights
    );

    Debug::dbg << "Starting new or loading existing game\n";
    wm.writeNewLine();
    wm.writeNewLine();
    giveDialog("Iintro");

    // controls + message
    wm.writeNewLine("Important infos : ");
    wm.writeNewLine(" - Scroll/Up/Down = move the game's text output");
    wm.writeNewLine(" - Escape = leave the game");
    wm.writeNewLine(" - Left/Right = change selected option");
    wm.writeNewLine(" - Enter = select current option");
    wm.writeNewLine(" - Most dialogs allow you to select a 'Home' option, go check it out !");
    wm.writeNewLine(" - Adventurers consume 1 ration per day");
    wm.writeNewLine();
    wm.writeNewLine(" - Enjoy !");
    wm.writeNewLine();

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

    // guild.adventurers.push_back(newRandomAdventurer());
    // guild.adventurers.push_back(newRandomAdventurer());
    // guild.adventurers.push_back(newRandomAdventurer());
    // guild.adventurers.push_back(newRandomAdventurer());
    
    // Mission mission = newRandomMission(10);
    // std::vector<std::string> advIdentifiers = guild.adventurerIdentifiers();

    // mission.assignedAdventurers = adventurerSelectionDialog();

    // guild.addMission(mission);

    // giveDialog("QRandomStuff", true);

    // finishMission(mission);

    // giveDialog("QRandomStuff", true);
    
    // giveDialog("QendDemo");

    while (!guild.hasLost())
    {
        Debug::dbg << "Starting phase " << std::to_string(guild.phase) << "\n";
        switch (guild.phase)
        {
        // phase food delivery
        case 0:
        {
            Debug::dbg << "Starting new day\n";
            giveDialog("IGoodMorning");
            wm.writeNewLine("Day " + std::to_string(guild.day) + " : ");
            wm.writeNewLine(gameUtil::fitStr("", 20, '-'));
            wm.writeNewLine();

            giveDialog("IMerchantIsHere");
            int foodAmount = numberDialog("NBuyFood", true);
            giveDialog("IFoodDelivery");
            
            // applying changes all at once
            guild.rations += foodAmount;
            guild.gold -= foodAmount;
            guild.phase++;
            break;
        }
        // phase salary distribution
        case 1:
        {
            giveDialog("ISalaries");
            Debug::dbg << "Distributing salaries\n";
            if (guild.adventurers.size() > 0)
            {
                for (auto &adv : guild.adventurers)
                {
                    int salary = adv.getSalary();
                    if (salary > guild.gold)
                    {
                        wm.writeNewLine(adv.name + " cannot be paid in full !");
                        wm.writeNewLine("-" + std::to_string(std::max(0, guild.gold)) + " Gold to " + adv.name);
                        guild.gold = std::min(guild.gold, 0);
                    }
                    else
                    {
                        wm.writeNewLine("-" + std::to_string(salary) + " Gold to " + adv.name);
                        guild.gold -= salary;
                    }
                }
                wm.writeNewLine();
            }
            else
            {
                giveDialog("INothingToDo");
            }
            guild.phase++;
            break;
        }
        // phase adventurer arrival
        case 2:
        {
            giveDialog("IMidday");
            giveDialog("INewArrivals");
            std::vector<Adventurer> new_adventurers{};
            for (int i = 0; i < DAILY_ADVENTURER_COUNT; i++)
            {
                new_adventurers.push_back(newRandomAdventurer());
            }
            renderAdventurers(new_adventurers);
            std::vector<std::string> accepted_adventurers = adventurerSelectionDialog(new_adventurers);
            for (auto &advIdentifier : accepted_adventurers)
            {
                for (auto &adv : new_adventurers)
                {
                    if (advIdentifier == adv.identifier)
                    {
                        guild.adventurers.push_back(adv);
                    }
                }
            }
            guild.phase++;
            break;
        }
        //phase mission attribution and results
        case 3:
        {
            Debug::dbg << "Generating new daily missions\n";
            giveDialog("INewMissions");
            if (guild.adventurers.size() > 0)
            {
                std::vector<Mission> daily_missions{};
                daily_missions.push_back(newRandomMission(guild.level));
                for (int i = 1; i < DAILY_MISSIONS_COUNT; i++)
                {
                    daily_missions.push_back(newRandomMission(guild.level + getDiceRoll(MISSION_MAX_ADD_LEVEL)));
                }
                Debug::dbg << "Showing new daily missions\n";
                missionSelectionDialog(daily_missions);
            }
            else
            {
                giveDialog("INothingToDo");
            }
            
            guild.phase++;
            break;
        }
        case 4:
            giveDialog("IEvening");
            advanceDay();
            giveDialog("ISurvived");
            guild.gold -= 10;
            wm.writeNewLine("-10 Gold for rent.");
            wm.writeNewLine();
            if (guild.gold < 0)
            {
                giveDialog("IDebt");
            }
            
            guild.phase = 0;
            break;
        default:
            break;
        }
    }
    giveDialog("ILost");
    exitGame(false);
}

// sends a dialog to the player based on the data found at dialogName in the data/dialogs.json file
int Game::giveDialog(const char *dialogName, bool _showHome, bool canQuit)
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
        if (_showHome)
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
            else if (option == options.size()-1 && _showHome)
            {
                wm.writeNewLine("> Home");
                this->showHome();
                wm.writeNewLine(dialog["text"].get<std::string>());
                wm.writeNewLine();
                option = -1;
            }
        }
        Debug::dbg << "Player selected option " << option << " : " << dialog["choices"][option] << "\n";
        wm.writeNewLine();
        wm.updateWindow();
        return option;
    }
    wm.writeNewLine();
    wm.updateWindow();
    return -1;
}

int Game::numberDialog(const char *dialogName, bool _showHome)
{
    Debug::dbg << "Giving number dialog: " << dialogName << "\n";
    nlohmann::json dialog = dialogs[dialogName];
    if (dialog.contains("text"))
    {
        wm.writeNewLine(dialog["text"].get<std::string>());
    }
    int min, max;
    if (dialog.contains("min"))
    {
        min = dialog["min"].get<int>();
    }
    else
    {
        min = std::numeric_limits<int>::min();
    }
    if (dialog.contains("max"))
    {
        max = dialog["max"].get<int>();
    }
    else
    {
        max = std::numeric_limits<int>::max();
    }
    int len = _showHome ? 8 : 7;
    std::string list[len];
    if (_showHome)
    {
        list[0] = "<<<";
        list[1] = "<<";
        list[2] = "<";
        list[4] = ">";
        list[5] = ">>";
        list[6] = ">>>";
        list[7] = "Home";
    }
    else
    {
        list[0] = "<<<";
        list[1] = "<<";
        list[2] = "<";
        list[4] = ">";
        list[5] = ">>";
        list[6] = ">>>";
    }
    
    int selected = 0;
    int number = 0;
    while (selected != 3)
    {
        list[3] = std::to_string(number);
        wm.writeNewLine();
        selected = gameUtil::chooseOption(wm, len, list, selected);
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
                else if(dialog.contains("text"))
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
        case 7:
            wm.writeNewLine("> Home");
            this->showHome();
            wm.writeNewLine(dialog["text"].get<std::string>());
        default:
            break;
        }
    }
    number = std::clamp(number, min, max);
    wm.writeNewLine("> " + std::to_string(number));
    wm.writeNewLine();
    wm.updateWindow();
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
    wm.writeNewLine();
    wm.updateWindow();
    return input;
}

std::vector<std::string> Game::adventurerPtrSelectionDialog(std::vector<Adventurer*> &adventurers, int max)
{
    Debug::dbg << "Giving adventurer selection dialog\n";
    if (max != -1)
    {
        wm.writeNewLine(dialogs["ISelectAdventurers"]["text"].get<std::string>() + " (max " + std::to_string(max) + ")");
    }
    else
    {
        wm.writeNewLine(dialogs["ISelectAdventurers"]["text"].get<std::string>());
    }
    
    wm.updateWindow();

    int pos = 0;
    int selected = 1;

    bool selected_adventurers[adventurers.size()];

    std::fill_n(selected_adventurers, adventurers.size(), false);
    int amount_selected = 0;
    int showing_len = std::min(static_cast<int>(adventurers.size()), MAX_OPTIONS_ADV_SELECTION);

    int options_len = showing_len + 4;
    std::string options[options_len];
    options[0] = "Done";
    options[1] = "<-";
    options[showing_len + 2] = "->";
    options[showing_len + 3] = "Home";

    while (selected != 0)
    {
        for (int i = 0; i < showing_len; i++)
        {
            options[i + 2] = (selected_adventurers[pos + i] ? "[*] " : "[ ] ") + adventurers[pos + i]->name;
        }
        wm.writeNewLine();
        selected = gameUtil::chooseOption(wm, options_len, options, selected);
        if (selected == 0)
        {
            continue;
        }
        else if (selected == 1)
        {
            if (pos + MAX_OPTIONS_ADV_SELECTION < showing_len)
            {
                pos++;
            }
        } 
        else if (selected == showing_len + 2)
        {
            if (pos > 0)
            {
                pos++;
            }
        }
        else if (selected == showing_len + 3)
        {
            wm.writeNewLine("> Home");
            showHome();
            wm.writeNewLine(dialogs["ISelectAdventurers"]["text"].get<std::string>());
            selected = 1;
        }
        
        else
        {
            int index = pos + selected - 2;
            if (selected_adventurers[index])
            {
                selected_adventurers[index] = false;
                amount_selected--;
            }
            else if (amount_selected < max || max == -1)
            {
                selected_adventurers[index] = true;
                amount_selected++;
            }
        }
    }
    std::vector<std::string> selected_indentifiers{};
    for (int i = 0; i < adventurers.size(); i++)
    {
        if (selected_adventurers[i])
        {
            selected_indentifiers.push_back(adventurers[i]->identifier);
        }
    }
    wm.writeNewLine("> Done");
    return selected_indentifiers;
}

std::vector<std::string> Game::adventurerSelectionDialog(std::vector<Adventurer> &adventurers, int max)
{
    std::vector<Adventurer*> adventurerPtrs{};
    for (auto &adv : adventurers)
    {
        adventurerPtrs.push_back(&adv);
    }
    return adventurerPtrSelectionDialog(adventurerPtrs, max);
}

//shows available mission, allows to select missions and select which adventurers to send
void Game::missionSelectionDialog(std::vector<Mission> &missions)
{
    Debug::dbg << "Giving mission selection dialog\n";

    std::vector<std::vector<std::string>> cardsLines;
    int index = 0;
    for (int i = 0; i < missions.size(); i++)
    {
        std::vector<std::string> card = toMissionCard(missions[i]);
        card.insert(card.begin(), std::to_string(i) + ".");
        cardsLines.push_back(card);
    }
    gameUtil::renderCards(wm, cardsLines);
    wm.writeNewLine();

    int selected = -1;
    while (selected != 0)
    {
        std::vector<Adventurer*> unoccupied_adventurers = guild.getUnoccupiedAdventurers();
        if (unoccupied_adventurers.size() <= 0)
        {
            giveDialog("INoUnoccupiedAdventurers");
            return;
        }
        if (missions.size() <= 0)
        {
            giveDialog("INoMoreMissions");
            return;
        }
        
        wm.writeNewLine(dialogs["ISelectMission"]["text"].get<std::string>());
        int options_len = missions.size() + 2;
        std::string options[options_len];
        options[0] = "Done";
        options[missions.size() + 1] = "Home";
        selected = -1;
        while (selected == -1)
        {
            wm.writeNewLine();
            for (int i = 0; i < missions.size(); i++)
            {
                options[i+1] = std::to_string(i);
            }
            
            selected = gameUtil::chooseOption(wm, options_len, options, 0, true);
            if (selected == 0)
            {
                continue;
            }
            else if (selected == missions.size() + 1)
            {
                showHome();
                wm.writeNewLine(dialogs["ISelectMission"]["text"].get<std::string>());
                selected = -1;
            }
            else
            {
                int index = selected-1;
                Debug::dbg << "Selected mission of index " << index << "\n";
                
                for (auto &advName : adventurerPtrSelectionDialog(unoccupied_adventurers, MAX_ADVENTURERS_PER_MISSION))
                {
                    std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(advName);
                    if (advOpt.has_value())
                    {
                        missions[index].assignAdventurer(*advOpt.value());
                    }
                }
                if (missions[index].assignedAdventurers.size() > 0)
                {
                    guild.addMission(missions[index]);
                    Debug::dbg << "Erasing mission\n";
                    missions.erase(missions.begin() + index);
                }
            }
        }
    }
    
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
    throw GameExit();
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
        adventurer_names["first_names"].size()
    )].get<std::string>();

    std::string advLastName = adventurer_names["last_names"][rng.uniformInt(
        0, 
        adventurer_names["last_names"].size()
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

    adv.salary = getDiceRoll(RANDOM_ADD_SALARY) + BASE_SALARY;

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
    if (level > 10)
    {
        int actual_level = 0;
        std::map<std::string, int> mission_monsters = std::map<std::string, int>();
        std::vector<std::string> mission_monsters_keys = std::vector<std::string>();
        
        while (actual_level != level)
        {
            int rand = rng.weightedInt(0, monsters.size(), monsters_weights);
            int monster_level = monsters[monsters_keys[rand]]["level"].get<int>();
            if (actual_level + monster_level < level)
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
                actual_level += monster_level;
            }
            else
            {
                int lowest_index = 0;
                int lowest_value = abs(actual_level + monsters[monsters_keys[0]]["level"].get<int>() - level);
                for (int i = 1; i < monsters_weights.size(); i++)
                {
                    int potential_lowest = abs(actual_level + monsters[monsters_keys[lowest_index]]["level"].get<int>() - level);
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
                actual_level += monsters[key]["level"].get<int>();
                break;
            }
        }
        int reward = actual_level * GOLD_ELIMINATION_MISSION_LEVEL_MUPLTIPLIER;
        int timeDays = static_cast<int>(std::sqrt(actual_level)) + 1;
        Mission mission(std::string("Elimination quest"), actual_level, reward, timeDays, mission_monsters);
        mission.terrainType = terrain_types_keys[rng.weightedInt(0, terrain_types.size(), terrain_types_weights)];
        mission.createIdentifier();
        return mission;
    }
    else
    {
        int reward = level * GOLD_TRAINING_MISSION_LEVEL_MUPLTIPLIER;
        Mission mission(std::string("Training quest"), level, reward, DAYS_COMPLETE_TRAINING_MISSION, std::map<std::string, int>());
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
    Debug::dbg << "Finishing mission\n";
    MissionResult result = determineSuccess(mission);
    giveDialog("IMissionResult");
    bool lostAdv = result.lostAdventurers.size() > 0;
    std::string lostAdvLine = "";
    if (lostAdv)
    {
        std::string advIdentifier;
        for (int i = 0; i < result.lostAdventurers.size()-1; i++)
        {
            advIdentifier = result.lostAdventurers[i];
            lostAdvLine += guild.getAdventurerByIdentifier(advIdentifier).value()->name + ", ";
            guild.removeAdventurerByIdentifier(advIdentifier);
            mission.removeAdventurer(advIdentifier);
        }
        advIdentifier = result.lostAdventurers[result.lostAdventurers.size()-1];
        lostAdvLine += guild.getAdventurerByIdentifier(advIdentifier).value()->name;
        guild.removeAdventurerByIdentifier(advIdentifier);
        mission.removeAdventurer(advIdentifier);
    }
    else
    {
        lostAdvLine = "None";
    }
    
    std::vector<std::string> card{
        std::string("Status: " + std::string(result.success ? "Success" : "Failure")),
        std::string("Lost adventurers: " + lostAdvLine),
        std::string("Gold gained: " + std::to_string(result.gainedGold)),
        std::string("Additional info: ")
    };

    for (auto &advIdentifier : mission.assignedAdventurers)
    {
        int roll = getDiceRoll(100);
        std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(advIdentifier);
        if (advOpt.has_value())
        {
            int neg_mod_chance = result.success ? NEGATIVE_MOD_CHANCE_SUCCESS : NEGATIVE_MOD_CHANCE_FAILURE;
            // chance of negative trait, range 0 to neg_mod_chance
            if (roll < neg_mod_chance)
            {
                std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(advIdentifier);
                if (advOpt.has_value())
                {
                    std::string mod = giveModWithBenefit(*advOpt.value(), -1);
                    if (!mod.empty())
                    {
                        card.push_back(" " + advOpt.value()->name + " has gained the negative trait '" + gameUtil::snakeToNormal(mod) + "' !");
                    }
                }
            }
            // chance of positive trait, range neg_mod_chance to POSITIVE_MOD_CHANCE
            else if (roll < neg_mod_chance + POSITIVE_MOD_CHANCE)
            {
                std::optional<Adventurer*> advOpt = guild.getAdventurerByIdentifier(advIdentifier);
                if (advOpt.has_value())
                {
                    std::string mod = giveModWithBenefit(*advOpt.value(), 1);
                    if (!mod.empty())
                    {
                        card.push_back(" " + advOpt.value()->name + " has gained the positive trait '" + gameUtil::snakeToNormal(mod) + "' !");
                    }
                }
            }
            std::pair<std::string, int> add_stat = giveAdditionalStat(*advOpt.value());
            card.push_back(" " + advOpt.value()->name + " has gained " + std::to_string(add_stat.second) + " " + add_stat.first + " !");

            // no longer occupied
            advOpt.value()->occupied = false;
        }
    }

    guild.gold += result.gainedGold;

    if (result.success)
    {
        guild.level += 2;
    }
    else
    {
        guild.level -= 1;
    }

    gameUtil::renderCards(wm, {card});
    wm.writeNewLine();
}

// renders the character cards of the given adventurers side by side
void Game::renderAdventurers(std::vector<std::string> adventurers)
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

void Game::renderAdventurers(std::vector<Adventurer> &adventurers)
{
    std::vector<std::vector<std::string>> cardsLines;
    for (int i = 0; i < adventurers.size(); i++)
    {
        std::vector<std::string> card = adventurers[i].toCharacterCard();
        cardsLines.push_back(card);
    }
    gameUtil::renderCards(wm, cardsLines);
}

void Game::renderAdventurers()
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
    int index = 0;
    for (int i = 0; i < missions.size(); i++)
    {
        std::vector<std::string> card = toMissionCard(missions[i]);
        cardsLines.push_back(card);
    }
    gameUtil::renderCards(wm, cardsLines);
}

void Game::showHome()
{
    Debug::dbg << "Showing home menu\n";
    int choice = giveDialog("QHome");
    switch (choice)
    {
    case 0:
        Debug::dbg << "Showing balance\n";
        gameUtil::renderCards(wm, {{"Current Balance: " + std::to_string(guild.gold) + " Gold"}});
        break;
    case 1:
        Debug::dbg << "Showing food stock\n";
        gameUtil::renderCards(wm, {{"Food stock: " + std::to_string(guild.rations) + " rations"}});
        break;
    case 2:
        Debug::dbg << "Showing day count\n";
        gameUtil::renderCards(wm, {{"Day " + std::to_string(guild.day)}});
        break;
    case 3:
        Debug::dbg << "Showing adventurer list\n";
        renderAdventurers();
        break;
    case 4:
        Debug::dbg << "Showing running missions\n";
        renderMissions(guild.missions);
        break;
    default:
        break;
    }
    wm.writeNewLine();
}

// this method attempts to find and assign a random modifier to the adventurer that matches the
// specified benefit value. may fail to find a valid modifier within 10 tries, in which case no
// modifiers will be applied.
std::string Game::giveModWithBenefit(Adventurer &adv, int benefit)
{
    Debug::dbg << "Trying to give modifier with benefit " << benefit << " to adventurer: " << adv.name << "\n";
    RNG &rng = RNG::get();
    int drawn = -1;
    for (int i = 0; i < 10;i++)
    {
        int rand = rng.weightedInt(0, adventurer_modifiers.size(), adventurer_modifiers_weights);
        std::string key = adventurer_modifiers_keys[rand];
        if (adventurer_modifiers[key]["benefit"].get<int>() != benefit)
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
        return adventurer_modifiers_keys[drawn];
    }
    return "";
}

// gives additional points (1 to MAX_ADDITIONAL_STAT_INCREASE) to a random stat
std::pair<std::string, int> Game::giveAdditionalStat(Adventurer &adv)
{
    int whichStat = getDiceRoll(STATS_COUNT);
    int amount = getDiceRoll(MAX_ADDITIONAL_STAT_INCREASE);
    std::pair<std::string, int> result;
    switch (whichStat)
    {
    case 1:
    {
        adv.stats.add_strength += amount;
        result = {"strength", amount};
        break;
    }
    case 2:
    {
        adv.stats.add_agility += amount;
        result = {"agility", amount};
        break;
    }
    case 3:
    {
        adv.stats.add_fortitude += amount;
        result = {"fortitude", amount};
        break;
    }
    case 4:
    {
        adv.stats.add_willpower += amount;
        result = {"willpower", amount};
        break;
    }
    case 5:
    {
        adv.stats.add_perception += amount;
        result = {"perception", amount};
        break;
    }
    case 6:
    {
        adv.stats.add_wisdom += amount;
        result = {"wisdom", amount};
        break;
    }
    case 7:
    {
        adv.stats.add_strength += amount;
        result = {"magic", amount};
        break;
    }
    default:
        break;
    }
    updateAdventurerStatus(adv);
    return result;
}

// Creates a mission card (text-based) for the mission
// Output is a vector of strings, each string being a line
std::vector<std::string> Game::toMissionCard(Mission &mission)
{
    Debug::dbg << "Creating mission card for mission: " << mission.description << "\n";
    std::vector<std::string> card = {
        std::string("Mission: " + mission.description),
        std::string("Time to complete: " + std::to_string(mission.days)),
        std::string("Level: " + std::to_string(mission.level)),
        std::string("Reward: " + std::to_string(mission.reward)),
        std::string("Terrain: " + gameUtil::snakeToNormal(mission.terrainType, true))
    };
    if (mission.assignedAdventurers.size() > 0)
    {
        card.push_back(std::string("Assigned Adventurers: "));
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
    }
    if (mission.monsters.size() > 0)
    {
        card.push_back(std::string("Monsters:"));
        for (const auto &[key, value] : mission.monsters)
        {
            card.push_back(" " + gameUtil::snakeToNormal(key, true) + " x" + std::to_string(value));
        }
    }
    
    return card;
}

void Game::advanceDay()
{
    Debug::dbg << "Passing new day\n";
    guild.day += 1;
    if (guild.missions.size() > 0)
    {
        TheGuild *guildPtr = &guild;
        WindowManager *wmPtr = &wm;
        guild.missions.erase(
            std::remove_if(
                guild.missions.begin(), 
                guild.missions.end(), 
                [guildPtr, wmPtr, this](Mission &mission) {
                    mission.days -= 1;
                    if (mission.days <= 0)
                    {
                        finishMission(mission);
                        Debug::dbg << "Removing mission " + mission.identifier << "\n";
                        return true;
                    }
                    return false;
                }
            ),
            guild.missions.end()
        );
    }

    if (guild.adventurers.size() > 0)
    {
        TheGuild *guildPtr = &guild;
        WindowManager *wmPtr = &wm;
        guild.adventurers.erase(
            std::remove_if(
                guild.adventurers.begin(), 
                guild.adventurers.end(), 
                [guildPtr, wmPtr](Adventurer &adv) {
                    if (guildPtr->rations >= 1)
                    {
                        Debug::dbg << "Feeding Adventurer " << adv.name << "\n";
                        if (!adv.occupied)
                        {
                        }
                        wmPtr->writeNewLine(adv.name + " has been fed !");
                        adv.satiation = 100;
                        guildPtr->rations -= 1;
                    }
                    else
                    {
                        adv.satiation -= NO_FOOD_SUBSTRACT;
                    }
    
                    bool died = adv.isStarved();
                    if (died)
                    {
                        Debug::dbg << "Adventurer " << adv.name << " died of starvation, removing\n";
                        wmPtr->writeNewLine(adv.name + " died of starvation...");
                    }
                    return died; 
                }
            ),
            guild.adventurers.end()
        );
        wm.writeNewLine();
    }
}
