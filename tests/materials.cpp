// Project regression tests for Book 1, Sections 10-11: metal and glass.
#include "material.h"
#include "sphere.h"
#include "hittable_list.h"
#include <limits>
#include <iostream>

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    // Section 10: exact mirror directions and fuzzy metal absorption.
    const vec3 normal(0, 1, 0);
    const vec3 incoming(1, -1, 0);
    const vec3 reflection = reflect(incoming, normal);
    check((reflection - vec3(1, 1, 0)).near_zero(), "Reflection must reverse normal component");
    check(std::abs(reflection.length() - incoming.length()) < 1e-12,
          "Reflection must preserve length");
    hit_record record;
    record.p = point3(2, 3, 4);
    record.normal = normal;
    ray scattered;
    color attenuation;
    const color albedo(0.8, 0.6, 0.2);
    metal polished(albedo, 0);
    check(polished.scatter(ray(point3(), incoming), record, attenuation, scattered),
          "Polished metal must reflect above surface");
    check((scattered.direction() - unit_vector(reflection)).near_zero(),
          "Zero fuzz must give exact mirror direction");
    check((scattered.origin() - record.p).near_zero() && (attenuation - albedo).near_zero(),
          "Scatter must preserve hit origin and material albedo");
    polished.scatter(ray(point3(), 7 * incoming), record, attenuation, scattered);
    check((scattered.direction() - unit_vector(reflection)).near_zero(),
          "Reflection must not depend on incoming ray length");
    metal negative_fuzz(albedo, -1);
    negative_fuzz.scatter(ray(point3(), incoming), record, attenuation, scattered);
    check((scattered.direction() - unit_vector(reflection)).near_zero(), "Negative fuzz must clamp to zero");

    metal rough(albedo, 3); // Values above one clamp to one.
    const ray grazing(point3(), unit_vector(vec3(1, -0.01, 0)));
    const vec3 ideal = reflect(grazing.direction(), normal);
    int accepted = 0, absorbed = 0;
    bool valid = true;
    for (int i = 0; i < 2000; ++i)
    {
        bool result = rough.scatter(grazing, record, attenuation, scattered);
        valid = valid && std::abs((scattered.direction() - ideal).length() - 1) < 1e-12;
        valid = valid && (result == (dot(scattered.direction(), normal) > 0));
        result ? ++accepted : ++absorbed;
    }
    check(valid && accepted > 0 && absorbed > 0,
          "Rough metal must clamp fuzz, scatter variably, and absorb below-surface directions");
    lambertian matte(albedo);
    check(matte.scatter(grazing, record, attenuation, scattered)
          && (attenuation - albedo).near_zero() && scattered.direction().y() > 0,
          "Diffuse material must scatter outward with its albedo");
    bool rejected_null = false;
    try { sphere invalid(point3(), 1, nullptr); }
    catch (const std::invalid_argument &) { rejected_null = true; }
    check(rejected_null, "Sphere must reject missing material");
    // Section 11: Snell's law and total internal reflection.
    const vec3 incident(0.6, -0.8, 0);
    const vec3 bent = refract(incident, normal, 1.0 / 1.5);
    check(std::abs(bent.x() - 0.4) < 1e-12 && std::abs(bent.length() - 1) < 1e-12,
          "Air-to-glass refraction must obey Snell's law and remain unit length");
    check((refract(bent, normal, 1.5) - incident).near_zero(),
          "Glass-to-air refraction must restore incident angle across parallel boundaries");
    check((refract(vec3(0, -1, 0), normal, 1.0 / 1.5) - vec3(0, -1, 0)).near_zero(),
          "Normal-incidence transmission must not bend");

    dielectric glass(1.5);
    record.front_face = false;
    const vec3 internal(0.9, -std::sqrt(0.19), 0);
    glass.scatter(ray(point3(), internal), record, attenuation, scattered);
    check((scattered.direction() - reflect(internal, normal)).near_zero(),
          "Glass-to-air above critical angle must always reflect");
    check((attenuation - color(1, 1, 1)).near_zero()
          && (scattered.origin() - record.p).near_zero(),
          "Clear glass must retain energy and launch at hit point");

    // Section 11: check both branches and the angular trend in Schlick reflection.
    auto sample_boundary = [&](bool front_face, const vec3 &direction, double ratio) {
        record.front_face = front_face;
        int reflections = 0;
        bool valid = true;
        const vec3 transmitted = refract(direction, normal, ratio);
        const vec3 reflected = reflect(direction, normal);
        for (int i = 0; i < 10000; ++i)
        {
            glass.scatter(ray(point3(), 3 * direction), record, attenuation, scattered);
            if (scattered.direction().y() > 0)
            {
                ++reflections;
                valid = valid && (scattered.direction() - reflected).near_zero();
            }
            else valid = valid && (scattered.direction() - transmitted).near_zero();
        }
        check(valid && reflections > 0 && reflections < 10000,
              "Both glass branches must use correct entering/exiting ratio and normalized input");
        return reflections;
    };
    const int straight = sample_boundary(true, vec3(0, -1, 0), 1.0 / 1.5);
    check(straight > 250 && straight < 550,
          "Normal-incidence glass reflection frequency must be near 4 percent");
    sample_boundary(true, incident, 1.0 / 1.5);
    sample_boundary(false, vec3(0.4, -std::sqrt(0.84), 0), 1.5);
    const int grazing_reflections = sample_boundary(true, unit_vector(vec3(1, -0.05, 0)), 1.0 / 1.5);
    check(grazing_reflections > 7000, "Schlick reflectance must rise strongly at grazing angles");

    // Section 11: the hollow sphere has four material crossings.
    auto shell = std::make_shared<dielectric>(1.5);
    auto cavity = std::make_shared<dielectric>(1.0 / 1.5);
    hittable_list hollow;
    hollow.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.5, shell));
    hollow.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.4, cavity));
    const ray through(point3(-1, 0, 0), vec3(0, 0, -1));
    const double expected_t[] = {0.5, 0.6, 1.4, 1.5};
    double minimum = 0.001;
    for (int i = 0; i < 4; ++i)
    {
        hit_record boundary;
        const bool hit = hollow.hit(through, interval(minimum, 100), boundary);
        check(hit && std::abs(boundary.t - expected_t[i]) < 1e-12
              && boundary.front_face == (i < 2)
              && boundary.mat == ((i == 0 || i == 3) ? shell : cavity),
              "Hollow sphere must expose all four correctly oriented material boundaries");
        minimum = boundary.t + 0.001;
    }
    for (double index : {0.0, -1.0, std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::quiet_NaN()})
    {
        bool rejected = false;
        try { dielectric invalid(index); }
        catch (const std::invalid_argument &) { rejected = true; }
        check(rejected, "Dielectric must reject nonpositive or nonfinite index");
    }
    return failures ? 1 : 0;
}
