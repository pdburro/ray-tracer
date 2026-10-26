#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// Section 4
class ray
{
public:
    ray() = default;
    ray(const point3 &start, const vec3 &heading, double time = 0)
        : start_(start), heading_(heading), time_(time) {}

    const point3 &origin() const { return start_; }
    const vec3 &direction() const { return heading_; }
    double time() const { return time_; }
    point3 at(double t) const { return start_ + t * heading_; }

private:
    point3 start_;
    vec3 heading_;
    double time_ = 0;
};

#endif
