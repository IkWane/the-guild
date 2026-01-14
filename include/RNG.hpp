#pragma once
#include <random>
#include <climits>
#include <vector>

class RNG 
{
public:
    static RNG &get(unsigned int seed = UINT_MAX)
    {
        static RNG instance(seed);
        return instance;
    }
    int uniformInt(int min, int max);
    int weightedInt(int min, int max, std::vector<int> weights);
private:
    explicit RNG(unsigned int seed);
    std::mt19937 generator;
};