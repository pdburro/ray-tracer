#include "material.h"
#include "sphere.h"
#include <iostream>

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
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
    return failures ? 1 : 0;
}
