#include "RNG.hpp"

RNG::RNG(unsigned int seed)
{
    generator = std::mt19937( seed == UINT_MAX ? std::random_device{}() : seed);
}

int RNG::uniformInt(int min, int max)
{
    std::uniform_int_distribution distrib(min, max);
    return distrib(generator);
}

int RNG::weightedInt(int min, int max, std::vector<int> weights)
{
    int len = max - min + 1;
    
    std::vector<int> values(len);
    for (int i = 0; i < len; i++)
    {
        values[i] = min + i;
    }
    std::piecewise_constant_distribution<> dist(std::begin(values), std::end(values), std::begin(weights));
    return dist(generator);
}
