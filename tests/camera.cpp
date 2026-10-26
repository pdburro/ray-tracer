#include "camera.h"
#include <sstream>
#include <vector>

class ray_capture : public hittable
{
public:
    mutable std::vector<ray> rays;
    bool hit(const ray &r, interval, hit_record &) const override
    {
        rays.push_back(r);
        return false;
    }
};

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    auto verify = [&](camera &cam, const vec3 &forward, const vec3 &right, const vec3 &up) {
        ray_capture scene;
        std::ostringstream output;
        auto *original = std::cout.rdbuf(output.rdbuf());
        cam.render(scene);
        std::cout.rdbuf(original);
        const int height = int(cam.image_width / cam.aspect_ratio);
        check(scene.rays.size() == static_cast<size_t>(cam.image_width * height * cam.samples_per_pixel),
              "Camera must emit requested number of primary rays");
        const double half_height = std::tan(cam.vfov * std::acos(-1.0) / 360);
        const double half_width = half_height * cam.image_width / height;
        bool valid = true;
        for (size_t index = 0; index < scene.rays.size(); ++index)
        {
            const int pixel = int(index) / cam.samples_per_pixel;
            const int i = pixel % cam.image_width;
            const int j = pixel / cam.image_width;
            const ray &r = scene.rays[index];
            const double distance = dot(r.direction(), forward);
            const double x = dot(r.direction(), right) / distance;
            const double y = dot(r.direction(), up) / distance;
            const double left = -half_width + 2 * half_width * i / cam.image_width;
            const double top = half_height - 2 * half_height * j / height;
            valid = valid && (r.origin() - cam.lookfrom).near_zero() && distance > 0
                && x >= left - 1e-12 && x <= left + 2 * half_width / cam.image_width + 1e-12
                && y <= top + 1e-12 && y >= top - 2 * half_height / height - 1e-12;
        }
        check(valid, "Ray origins and jittered pixel bounds must match camera position, axes, and FOV");
    };
    camera cam;
    cam.image_width = 8;
    cam.aspect_ratio = 2;
    cam.samples_per_pixel = 4;
    verify(cam, vec3(0, 0, -1), vec3(1, 0, 0), vec3(0, 1, 0));
    cam.lookfrom = point3(3, 2, 1);
    cam.lookat = point3(4, 2, 1);
    cam.vfov = 20;
    verify(cam, vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0));
    cam.vup = vec3(0, 0, 7); // Roll camera and verify up need not be unit length.
    cam.vfov = 60;
    verify(cam, vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1));
    cam.lookat = point3(13, 2, 1); // Target distance must not change angular pixel bounds.
    verify(cam, vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1));

    for (int scenario = 0; scenario < 9; ++scenario)
    {
        camera invalid;
        if (scenario == 0) invalid.vfov = 0;
        if (scenario == 1) invalid.vfov = 180;
        if (scenario == 2) invalid.lookat = invalid.lookfrom;
        if (scenario == 3) invalid.vup = vec3(0, 0, -1);
        if (scenario == 4) invalid.vup = vec3();
        if (scenario == 5) invalid.aspect_ratio = 0;
        if (scenario == 6) invalid.image_width = 0;
        if (scenario == 7) invalid.lookfrom[0] = std::numeric_limits<double>::quiet_NaN();
        if (scenario == 8) invalid.vfov = std::numeric_limits<double>::infinity();
        bool rejected = false;
        ray_capture scene;
        try { invalid.render(scene); }
        catch (const std::invalid_argument &) { rejected = true; }
        check(rejected && scene.rays.empty(), "Invalid settings must fail before ray generation");
    }
    return failures ? 1 : 0;
}
