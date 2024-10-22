#include "rand.h"

uint32_t rand_seed = 0;

void srand(uint32_t seed)
{
    rand_seed = seed;
}

uint32_t rand()
{
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}