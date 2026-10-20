#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

// Section 3
using color = vec3;

inline void write_color(std::ostream &out, const color &pixel)
{
    out << static_cast<int>(255.999 * pixel.x()) << ' '
        << static_cast<int>(255.999 * pixel.y()) << ' '
        << static_cast<int>(255.999 * pixel.z()) << '\n';
}

#endif
