#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include <algorithm>

class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray &incoming, const hit_record &record,
                         color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material
{
public:
    explicit lambertian(const color &albedo) : albedo_(albedo) {}

    bool scatter(const ray &, const hit_record &record,
                 color &attenuation, ray &scattered) const override
    {
        scattered = ray(record.p, lambertian_direction(record.normal));
        attenuation = albedo_;
        return true;
    }

private:
    color albedo_;
};

class metal : public material
{
public:
    metal(const color &albedo, double fuzz) : albedo_(albedo), fuzz_(std::clamp(fuzz, 0.0, 1.0)) {}

    bool scatter(const ray &incoming, const hit_record &record,
                 color &attenuation, ray &scattered) const override
    {
        const vec3 reflected = reflect(unit_vector(incoming.direction()), record.normal);
        const vec3 direction = reflected + fuzz_ * random_unit_vector();
        scattered = ray(record.p, direction);
        attenuation = albedo_;
        return dot(direction, record.normal) > 0;
    }

private:
    color albedo_;
    double fuzz_;
};

#endif
