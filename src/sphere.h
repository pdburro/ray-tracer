#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include <stdexcept>

class sphere : public hittable
{
public:
    sphere(const point3 &center, double radius) : center_(center), radius_(radius)
    {
        if (!(radius > 0))
            throw std::invalid_argument("Sphere radius must be positive");
    }

    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        const vec3 to_center = center_ - r.origin();
        const double a = r.direction().length_squared();
        if (a == 0) return false;
        const double h = dot(r.direction(), to_center);
        const double c = to_center.length_squared() - radius_ * radius_;
        const double discriminant = h * h - a * c;
        if (discriminant < 0) return false;

        const double root_discriminant = std::sqrt(discriminant);
        double root = (h - root_discriminant) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + root_discriminant) / a;
            if (!ray_t.surrounds(root)) return false;
        }

        record.t = root;
        record.p = r.at(root);
        record.set_face_normal(r, (record.p - center_) / radius_);
        return true;
    }

private:
    point3 center_;
    double radius_;
};

#endif
