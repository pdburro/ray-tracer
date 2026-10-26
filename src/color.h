#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <algorithm>

// Section 3
using color = vec3;

inline double linear_to_gamma(double component)
{
    return component > 0 ? std::sqrt(component) : 0;
}

inline void write_color(std::ostream &out, const color &pixel)
{
    // Average samples in linear space first; encode for display only here.
    const auto to_byte = [](double component) {
        return static_cast<int>(256 * std::clamp(linear_to_gamma(component), 0.0, 0.999));
    };
    out << to_byte(pixel.x()) << ' ' << to_byte(pixel.y()) << ' '
        << to_byte(pixel.z()) << '\n';
}

#endif
