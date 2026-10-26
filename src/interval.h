#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

// Book 1, Section 6: bounds on the ray parameter t.
class interval
{
public:
    double min, max;
    // The default interval is empty: its lower bound exceeds its upper bound.
    interval() : min(+std::numeric_limits<double>::infinity()),
                 max(-std::numeric_limits<double>::infinity()) {}
    interval(double minimum, double maximum) : min(minimum), max(maximum) {}
    // Section 6: contains includes endpoints; surrounds excludes them.
    bool contains(double x) const { return min <= x && x <= max; }
    bool surrounds(double x) const { return min < x && x < max; }
};

#endif
