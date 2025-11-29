#pragma once
#include "magicNumbers.hpp"
#include "json.hpp"
#include <ncurses.h>
#include <RNG.hpp>

namespace gameUtil
{
    int wrapAround(int a, int n);

    std::string readStr();

    int chooseOption(int optionsLen, std::string options[], int defaultChoice = 0);

    int chooseOption(nlohmann::json options, int defaultChoice = 0);

    int giveDialog(nlohmann::json dialog);

    int numberDialog(nlohmann::json dialog);

    std::string textEntryDialog(nlohmann::json dialog);

    int getDiceRoll(int max);
} // namespace gameUtil