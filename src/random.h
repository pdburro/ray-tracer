#ifndef RANDOM_H
#define RANDOM_H

#include <random>

// Fixed initial seed makes repeated runs reproducible on the same toolchain.
inline double random_double()
{
    static std::mt19937 generator(5489u);
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator); // [0, 1)
}

#endif
