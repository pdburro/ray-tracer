#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include <stdexcept>

class sphere : public hittable
{
public:
    sphere(const point3 &center, double radius, std::shared_ptr<material> mat)
        : sphere(center, center, radius, mat) {}

    sphere(const point3 &start, const point3 &end, double radius, std::shared_ptr<material> mat)
        : center_(start, end - start), radius_(radius), mat_(mat)
    {
        if (!(radius > 0))
            throw std::invalid_argument("Sphere radius must be positive");
        if (!mat_)
            throw std::invalid_argument("Sphere material must not be null");
        const vec3 offset(radius, radius, radius);
        box_ = aabb(aabb(start - offset, start + offset), aabb(end - offset, end + offset));
    }

    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        const point3 current_center = center_.at(r.time());
        const vec3 to_center = current_center - r.origin();
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
        record.mat = mat_;
        record.set_face_normal(r, (record.p - current_center) / radius_);
        return true;
    }

    aabb bounding_box() const override { return box_; }

private:
    aabb box_;
    ray center_;
    double radius_;
    std::shared_ptr<material> mat_;
};

#endif
