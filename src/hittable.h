#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "ray.h"
#include <memory>

// Section 10: a forward declaration keeps material and hit records from including each other.
class material;

// Book 1, Section 6: intersection data; Section 10 adds the material pointer.
struct hit_record
{
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t = 0;
    bool front_face = false;

    // Section 6: keep the normal opposed to the ray and remember entry/exit.
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Section 6: one intersection interface for individual objects and whole scenes.
class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_record &record) const = 0;
};

#endif
