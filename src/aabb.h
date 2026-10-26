#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include <algorithm>

class aabb
{
public:
    interval x, y, z;

    aabb() = default;
    aabb(const point3 &a, const point3 &b)
        : x(std::min(a.x(), b.x()), std::max(a.x(), b.x())),
          y(std::min(a.y(), b.y()), std::max(a.y(), b.y())),
          z(std::min(a.z(), b.z()), std::max(a.z(), b.z()))
    {
        pad();
    }
    aabb(const aabb &a, const aabb &b)
        : x(std::min(a.x.min, b.x.min), std::max(a.x.max, b.x.max)),
          y(std::min(a.y.min, b.y.min), std::max(a.y.max, b.y.max)),
          z(std::min(a.z.min, b.z.min), std::max(a.z.max, b.z.max)) {}

    const interval &axis_interval(int axis) const
    {
        if (axis == 1) return y;
        if (axis == 2) return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const
    {
        if (ray_t.max < ray_t.min) return false;
        for (int axis = 0; axis < 3; ++axis)
        {
            const interval &slab = axis_interval(axis);
            if (slab.max < slab.min) return false;
            const double direction = r.direction()[axis];
            if (direction == 0)
            {
                if (!slab.contains(r.origin()[axis])) return false;
                continue;
            }
            double near = (slab.min - r.origin()[axis]) / direction;
            double far = (slab.max - r.origin()[axis]) / direction;
            if (near > far) std::swap(near, far);
            ray_t.min = std::max(ray_t.min, near);
            ray_t.max = std::min(ray_t.max, far);
            if (ray_t.max < ray_t.min) return false;
        }
        return true;
    }

    int longest_axis() const
    {
        if (x.size() > y.size()) return x.size() > z.size() ? 0 : 2;
        return y.size() > z.size() ? 1 : 2;
    }

private:
    void pad()
    {
        const double minimum_width = 0.0001;
        if (x.size() < minimum_width) x = x.expand(minimum_width);
        if (y.size() < minimum_width) y = y.expand(minimum_width);
        if (z.size() < minimum_width) z = z.expand(minimum_width);
    }
};

#endif
