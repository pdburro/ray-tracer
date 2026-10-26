#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include "texture.h"
#include <algorithm>
#include <stdexcept>

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
    explicit lambertian(const color &albedo) : lambertian(std::make_shared<solid_color>(albedo)) {}
    explicit lambertian(std::shared_ptr<texture> tex) : texture_(tex)
    {
        if (!tex) throw std::invalid_argument("Diffuse material needs a texture");
    }

    bool scatter(const ray &incoming, const hit_record &record,
                 color &attenuation, ray &scattered) const override
    {
        scattered = ray(record.p, lambertian_direction(record.normal), incoming.time());
        attenuation = texture_->value(record.u, record.v, record.p);
        return true;
    }

private:
    std::shared_ptr<texture> texture_;
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
        scattered = ray(record.p, direction, incoming.time());
        attenuation = albedo_;
        return dot(direction, record.normal) > 0;
    }

private:
    color albedo_;
    double fuzz_;
};

class dielectric : public material
{
public:
    // Index of this object's medium divided by that of its enclosing medium.
    explicit dielectric(double refraction_index) : refraction_index_(refraction_index)
    {
        if (!(refraction_index > 0) || !std::isfinite(refraction_index))
            throw std::invalid_argument("Refraction index must be finite and positive");
    }

    bool scatter(const ray &incoming, const hit_record &record,
                 color &attenuation, ray &scattered) const override
    {
        attenuation = color(1, 1, 1); // Clear, nonabsorbing glass.
        const double ratio = record.front_face ? 1.0 / refraction_index_ : refraction_index_;
        const vec3 unit_direction = unit_vector(incoming.direction());
        const double cosine = std::clamp(dot(-unit_direction, record.normal), 0.0, 1.0);
        const double sine = std::sqrt(1.0 - cosine * cosine);
        const bool cannot_refract = ratio * sine > 1.0;
        vec3 direction;
        if (cannot_refract || reflectance(cosine, ratio) > random_double())
            direction = reflect(unit_direction, record.normal);
        else
            direction = refract(unit_direction, record.normal, ratio);
        scattered = ray(record.p, direction, incoming.time());
        return true;
    }

private:
    double refraction_index_;

    static double reflectance(double cosine, double ratio)
    {
        double r0 = (1 - ratio) / (1 + ratio);
        r0 *= r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
};

#endif
