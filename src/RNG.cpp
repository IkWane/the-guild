#include "RNG.hpp"
#include <iostream>

int rPrint()
{
    std::cout << "test" << std::endl;
    return std::random_device{}();
}

RNG::RNG(unsigned int seed)
{
    generator = std::mt19937(seed);
}

int RNG::uniformInt(int min, int max)
{
    std::uniform_int_distribution distrib(min, max);
    return distrib(generator);
}

