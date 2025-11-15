#include <random>

int rPrint();

class RNG 
{
public:
    static RNG &get(unsigned int seed = rPrint())
    {
        static RNG instance(seed);
        return instance;
    }
    int uniformInt(int min, int max);
private:
    explicit RNG(unsigned int seed);
    std::mt19937 generator;
};