#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// Book 1, Section 4: a ray is an origin plus a direction scaled by t.
class ray
{
public:
    ray() = default;
    ray(const point3 &start, const vec3 &heading) : start_(start), heading_(heading) {}

    const point3 &origin() const { return start_; }
    const vec3 &direction() const { return heading_; }
    // Section 4: P(t) = origin + t * direction; direction need not be unit length.
    point3 at(double t) const { return start_ + t * heading_; }

private:
    point3 start_;
    vec3 heading_;
};

#endif
