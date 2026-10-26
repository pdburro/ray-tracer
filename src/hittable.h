#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "ray.h"
#include "aabb.h"
#include <memory>

class material;

struct hit_record
{
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t = 0;
    bool front_face = false;

    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_record &record) const = 0;
    virtual aabb bounding_box() const = 0;
};

#endif
