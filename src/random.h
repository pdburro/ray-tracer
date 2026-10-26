#ifndef RANDOM_H
#define RANDOM_H

#include <random>

// Book 1, Section 8: random values for pixel sampling (also used in Sections 9-14).
// Fixed initial seed makes repeated runs reproducible on the same toolchain.
inline double random_double()
{
    static std::mt19937 generator(5489u);
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator); // [0, 1)
}

// Sections 9 and 14: scale the sample to [minimum, maximum).
inline double random_double(double minimum, double maximum)
{
    return minimum + (maximum - minimum) * random_double();
}

#endif
