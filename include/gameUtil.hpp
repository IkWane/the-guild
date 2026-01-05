#pragma once
#include "magicNumbers.hpp"
#include "json.hpp"
#include "RNG.hpp"
#include "FileManager.hpp"
#include <ncurses.h>
#include "Debug.hpp"
#include "Stats.hpp"
#include "Adventurer.hpp"

namespace gameUtil
{
    int wrapAround(int a, int n);

    int sigmoid(int x, int steepness = 1);

    std::string readStr();

    int chooseOption(int optionsLen, std::string options[], int defaultChoice = 0, bool giveResult = false);

    int chooseOption(nlohmann::json options, int defaultChoice = 0, bool giveResult = false);

    std::string snakeToNormal(std::string &str);

    void loadJsonConfig(std::string path, nlohmann::json &data, std::vector<std::string> &keys, std::vector<int> &weights);

    std::string fitStr(std::string str, int length);

    void renderCharacterCards(std::vector<std::vector<std::string>> cardsLines);
} // namespace gameUtil