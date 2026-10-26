#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include <algorithm>
#include <stdexcept>

// Book 1, Section 10: materials choose a scattered ray and RGB attenuation.
class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray &incoming, const hit_record &record,
                         color &attenuation, ray &scattered) const = 0;
};

// Sections 9-10: matte reflection with per-channel albedo.
class lambertian : public material
{
public:
    explicit lambertian(const color &albedo) : albedo_(albedo) {}

    // The diffuse model does not depend on the incoming direction.
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

// Section 10: mirror reflection with an optional random fuzz offset.
class metal : public material
{
public:
    metal(const color &albedo, double fuzz) : albedo_(albedo), fuzz_(std::clamp(fuzz, 0.0, 1.0)) {}

    bool scatter(const ray &incoming, const hit_record &record,
                 color &attenuation, ray &scattered) const override
    {
        // Normalize first so fuzz has the same scale for all incoming rays.
        const vec3 reflected = reflect(unit_vector(incoming.direction()), record.normal);
        const vec3 direction = reflected + fuzz_ * random_unit_vector();
        scattered = ray(record.p, direction);
        attenuation = albedo_;
        // Absorb directions that fuzz sends below the surface.
        return dot(direction, record.normal) > 0;
    }

private:
    color albedo_;
    double fuzz_;
};

// Section 11: clear glass, refraction, and angle-dependent reflection.
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
        // front_face distinguishes entry from exit at the material boundary.
        const double ratio = record.front_face ? 1.0 / refraction_index_ : refraction_index_;
        const vec3 unit_direction = unit_vector(incoming.direction());
        const double cosine = std::clamp(dot(-unit_direction, record.normal), 0.0, 1.0);
        const double sine = std::sqrt(1.0 - cosine * cosine);
        const bool cannot_refract = ratio * sine > 1.0;
        vec3 direction;
        // Total reflection is mandatory; otherwise sample one branch using Schlick.
        if (cannot_refract || reflectance(cosine, ratio) > random_double())
            direction = reflect(unit_direction, record.normal);
        else
            direction = refract(unit_direction, record.normal, ratio);
        scattered = ray(record.p, direction);
        return true;
    }

private:
    double refraction_index_;

    // Section 11: Schlick's approximation rises toward 1 at grazing angles.
    static double reflectance(double cosine, double ratio)
    {
        double r0 = (1 - ratio) / (1 + ratio);
        r0 *= r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
};

#endif
