#include "camera.h"
#include "sphere.h"
#include <sstream>
#include <vector>

class shutter_capture : public hittable
{
public:
    aabb bounding_box() const override { return aabb(); }
    mutable std::vector<double> times;
    bool hit(const ray &r, interval, hit_record &) const override
    {
        times.push_back(r.time());
        return false;
    }
};

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    check(ray().time() == 0 && ray(point3(), vec3(0, 0, -1)).time() == 0,
          "Existing ray constructors must default to time zero");
    auto gray = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    sphere moving(point3(0, 0, -3), point3(2, 0, -3), 0.5, gray);
    sphere stationary(point3(0, 0, -3), 0.5, gray);
    hit_record record;
    for (double time : {0.0, 0.5, 1.0})
    {
        const ray aimed(point3(2 * time, 0, 0), vec3(0, 0, -1), time);
        check(moving.hit(aimed, interval(0.001, 100), record)
              && std::abs(record.t - 2.5) < 1e-12 && record.normal.z() == 1,
              "Moving sphere must interpolate center and normal at ray time");
        check(stationary.hit(ray(point3(), vec3(0, 0, -1), time), interval(0.001, 100), record)
              && record.t == 2.5, "Stationary sphere must not change with time");
    }
    check(!moving.hit(ray(point3(), vec3(0, 0, -1), 1), interval(0.001, 100), record),
          "Ray through initial position must miss after sphere moves away");
    record.p = point3(0, 0, -2.5);
    record.normal = vec3(0, 0, 1);
    record.front_face = true;
    const ray incoming(point3(), vec3(0, 0, -1), 0.375);
    metal reflective(color(0.8, 0.8, 0.8), 0.3);
    dielectric glass(1.5);
    for (const material *surface : {static_cast<const material *>(gray.get()),
                                    static_cast<const material *>(&reflective),
                                    static_cast<const material *>(&glass)})
    {
        for (int i = 0; i < 100; ++i)
        {
            color attenuation;
            ray scattered;
            surface->scatter(incoming, record, attenuation, scattered);
            check(scattered.time() == incoming.time(), "Every material must preserve path time");
        }
    }
    camera cam;
    cam.image_width = 2;
    cam.samples_per_pixel = 32;
    shutter_capture capture;
    std::ostringstream image;
    auto *original = std::cout.rdbuf(image.rdbuf());
    cam.render(capture);
    std::cout.rdbuf(original);
    bool valid_times = capture.times.size() == 128;
    double minimum = 1, maximum = 0;
    for (double time : capture.times)
    {
        valid_times = valid_times && time >= 0 && time < 1;
        minimum = std::min(minimum, time);
        maximum = std::max(maximum, time);
    }
    check(valid_times && minimum < 0.1 && maximum > 0.9,
          "Camera must sample times across the shutter interval");
    return failures ? 1 : 0;
}
