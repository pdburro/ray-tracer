#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include <cmath>
#include <iostream>

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    auto gray = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    sphere ball(point3(0, 0, -1), 0.5, gray);
    hit_record record;
    const interval forward(0, 1000);
    const ray center(point3(0, 0, 0), vec3(0, 0, -2));
    check(ball.hit(center, forward, record), "Front ray must hit");
    check(std::abs(record.t - 0.25) < 1e-12, "Non-unit direction must give correct t");
    check(record.mat == gray, "Hit must retain assigned material");
    check(record.front_face && record.normal.z() == 1, "Outside normal must face ray");
    check(!ball.hit(ray(point3(), vec3(0, 0, 1)), forward, record), "Reject behind-camera roots");
    check(ball.hit(center, interval(0.3, 1), record) && record.t == 0.75,
          "Try far root when near root is outside interval");
    check(!ball.hit(center, interval(0.25, 0.75), record), "Exclude interval endpoints");
    check(ball.hit(ray(point3(0, 0, -1), vec3(0, 1, 0)), forward, record), "Inside ray must exit");
    check(!record.front_face && record.normal.y() == -1, "Inside normal must face ray");
    check(!ball.hit(ray(point3(), vec3()), forward, record), "Reject zero direction");
    check(ball.hit(ray(point3(0.5, 0, 0), vec3(0, 0, -1)), forward, record), "Tangent must hit");
    hittable_list world;
    check(!world.hit(center, forward, record), "Empty scene must miss");
    world.add(std::make_shared<sphere>(point3(0, 0, -3), 0.5, gray));
    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, gray));
    check(world.hit(center, forward, record) && record.t == 0.25, "Keep nearest object");
    world.clear();
    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5, gray));
    world.add(std::make_shared<sphere>(point3(0, 0, -3), 0.5, gray));
    check(world.hit(center, forward, record) && record.t == 0.25, "Nearest must ignore insertion order");
    return failures ? 1 : 0;
}
