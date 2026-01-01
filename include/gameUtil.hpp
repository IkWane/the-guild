#pragma once
#include "magicNumbers.hpp"
#include "json.hpp"
#include <ncurses.h>
#include <RNG.hpp>

namespace gameUtil
{
    int wrapAround(int a, int n);

    std::string readStr();

    int chooseOption(int optionsLen, std::string options[], int defaultChoice = 0, bool giveResult = false);

    int chooseOption(nlohmann::json options, int defaultChoice = 0, bool giveResult = false);

    std::string snakeToNormal(std::string &str);
} // namespace gameUtil