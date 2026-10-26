#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include <stdexcept>

// Book 1, Sections 5-6: sphere roots and normals; Section 10 adds a material.
class sphere : public hittable
{
public:
    // Section 10: every rendered sphere carries its own material reference.
    sphere(const point3 &center, double radius, std::shared_ptr<material> mat)
        : center_(center), radius_(radius), mat_(mat)
    {
        if (!(radius > 0))
            throw std::invalid_argument("Sphere radius must be positive");
        if (!mat_)
            throw std::invalid_argument("Sphere material must not be null");
    }

    // Sections 5-6: solve for the nearest root inside the allowed ray interval.
    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        const vec3 to_center = center_ - r.origin();
        const double a = r.direction().length_squared();
        if (a == 0) return false;
        // Section 6: h replaces -b/2 in the quadratic formula.
        const double h = dot(r.direction(), to_center);
        const double c = to_center.length_squared() - radius_ * radius_;
        const double discriminant = h * h - a * c;
        if (discriminant < 0) return false;

        const double root_discriminant = std::sqrt(discriminant);
        double root = (h - root_discriminant) / a;
        if (!ray_t.surrounds(root))
        {
            // A ray inside the sphere needs the farther root to find its exit.
            root = (h + root_discriminant) / a;
            if (!ray_t.surrounds(root)) return false;
        }

        record.t = root;
        record.p = r.at(root);
        record.mat = mat_;
        record.set_face_normal(r, (record.p - center_) / radius_);
        return true;
    }

private:
    point3 center_;
    double radius_;
    std::shared_ptr<material> mat_;
};

#endif
