#pragma once
#include <random>
#include <climits>

class RNG 
{
public:
    static RNG &get(unsigned int seed = UINT_MAX)
    {
        static RNG instance(seed);
        return instance;
    }
    int uniformInt(int min, int max);
private:
    explicit RNG(unsigned int seed);
    std::mt19937 generator;
};