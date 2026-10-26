#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

class interval
{
public:
    double min, max;
    interval() : min(+std::numeric_limits<double>::infinity()),
                 max(-std::numeric_limits<double>::infinity()) {}
    interval(double minimum, double maximum) : min(minimum), max(maximum) {}
    bool contains(double x) const { return min <= x && x <= max; }
    bool surrounds(double x) const { return min < x && x < max; }
};

#endif
