#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// Section 4
class ray
{
public:
    ray() = default;
    ray(const point3 &start, const vec3 &heading) : start_(start), heading_(heading) {}

    const point3 &origin() const { return start_; }
    const vec3 &direction() const { return heading_; }
    point3 at(double t) const { return start_ + t * heading_; }

private:
    point3 start_;
    vec3 heading_;
};

#endif
