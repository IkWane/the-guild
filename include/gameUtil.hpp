#pragma once
#include "magicNumbers.hpp"
#include "json.hpp"
#include "RNG.hpp"
#include "FileManager.hpp"
#include <ncurses.h>

namespace gameUtil
{
    int wrapAround(int a, int n);

    std::string readStr();

    int chooseOption(int optionsLen, std::string options[], int defaultChoice = 0, bool giveResult = false);

    int chooseOption(nlohmann::json options, int defaultChoice = 0, bool giveResult = false);

    std::string snakeToNormal(std::string &str);

    void loadJsonConfig(std::string path, nlohmann::json &data, std::vector<std::string> &keys, std::vector<int> &weights);
} // namespace gameUtil