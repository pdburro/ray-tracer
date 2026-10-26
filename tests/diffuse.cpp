// Project regression tests for Book 1, Sections 9-10: sampling, gamma, and path depth.
#include "camera.h"
#include <sstream>

// Section 9: always return a hit so the test can count bounces before termination.
class endless_scene : public hittable
{
public:
    mutable int hits = 0;
    mutable bool valid_interval = true;
    bool hit(const ray &r, interval range, hit_record &record) const override
    {
        ++hits;
        valid_interval = valid_interval && range.min == 0.001;
        record.p = r.at(1);
        record.normal = vec3(0, 1, 0);
        record.mat = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
        return true;
    }
};

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    vec3 mean;
    double cosine_sum = 0;
    bool valid_directions = true;
    constexpr int count = 20000;
    for (int i = 0; i < count; ++i)
    {
        const vec3 u = random_unit_vector();
        mean += u;
        valid_directions = valid_directions && std::abs(u.length() - 1) < 1e-12;
        const vec3 d = lambertian_direction(vec3(0, 1, 0));
        valid_directions = valid_directions && !d.near_zero() && d.y() >= 0;
        cosine_sum += unit_vector(d).y();
    }
    check(valid_directions, "Samples must be unit vectors / outward nonzero bounces");
    check((mean / count).length() < 0.02, "Unit directions must not have a strong directional bias");
    check(std::abs(cosine_sum / count - 2.0 / 3) < 0.02,
          "Lambertian samples must follow cosine-weighted rather than uniform scattering");
    check(linear_to_gamma(-1) == 0 && linear_to_gamma(0.25) == 0.5,
          "Gamma must handle negative values and square-root positive values");
    std::ostringstream bytes;
    write_color(bytes, color(-1, 0.25, 4));
    check(bytes.str() == "0 128 255\n", "Gamma output must clamp RGB bytes");

    camera cam;
    cam.image_width = 1;
    cam.samples_per_pixel = 2;
    cam.max_depth = 4;
    endless_scene world;
    std::ostringstream image;
    auto *original = std::cout.rdbuf(image.rdbuf());
    cam.render(world);
    std::cout.rdbuf(original);
    check(world.hits == 8 && world.valid_interval, "Each path must obey depth and hit tolerance");
    check(image.str() == "P3\n1 1\n255\n0 0 0\n", "Terminated paths must contribute black");
    return failures ? 1 : 0;
}
