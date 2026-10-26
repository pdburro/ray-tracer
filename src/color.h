#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <algorithm>

// Book 1, Section 3: RGB colors use the vector type.
using color = vec3;

// Section 9: approximate gamma-2 encoding for display.
inline double linear_to_gamma(double component)
{
    return component > 0 ? std::sqrt(component) : 0;
}

// Sections 2-3: PPM pixel output; Sections 8-9: clamp and encode averaged colors.
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
