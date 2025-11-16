#include "RNG.hpp"
#include <iostream>

RNG::RNG(unsigned int seed)
{
    std::cout << seed << std::endl;
    generator = std::mt19937( seed == UINT_MAX ? std::random_device{}() : seed);
}

int RNG::uniformInt(int min, int max)
{
    std::uniform_int_distribution distrib(min, max);
    return distrib(generator);
}

