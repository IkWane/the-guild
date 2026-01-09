#pragma once
#include "magicNumbers.hpp"
#include "json.hpp"
#include "RNG.hpp"
#include "FileManager.hpp"
#include <ncurses.h>
#include "Debug.hpp"
#include "Stats.hpp"
#include "Adventurer.hpp"
#include "WindowManager.hpp"

namespace gameUtil
{
    int wrapAround(int a, int n);

    float sigmoid(int x, float steepness = 1);

    std::string readStr(WindowManager &wm);

    int chooseOption(WindowManager &wm, int optionsLen, std::string options[], int defaultChoice = 0, bool giveResult = false);

    int chooseOption(WindowManager &wm, nlohmann::json options, int defaultChoice = 0, bool giveResult = false);

    std::string snakeToNormal(std::string &str, bool upperFirst = false);

    void loadJsonConfig(std::string path, nlohmann::json &data, std::vector<std::string> &keys, std::vector<int> &weights);

    std::string fitStr(std::string str, int length, char fillChar = ' ');

    void renderCards(WindowManager &wm, std::vector<std::vector<std::string>> cardsLines);
} // namespace gameUtil