#include "RNG.hpp"

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

// int RNG::weightedInt(int min, int max, float weights[])
// {

//     std::piecewise_constant_distribution<> dist();
//     return 0;
// }
