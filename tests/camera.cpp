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
        double origin_spread = 0;
        const double lens_radius = cam.focus_dist * std::tan(cam.defocus_angle * std::acos(-1.0) / 360);
        for (size_t index = 0; index < scene.rays.size(); ++index)
        {
            const int pixel = int(index) / cam.samples_per_pixel;
            const int i = pixel % cam.image_width;
            const int j = pixel / cam.image_width;
            const ray &r = scene.rays[index];
            const double distance = dot(r.direction(), forward);
            const vec3 offset = r.origin() - cam.lookfrom;
            // Intersect the emitted ray with the focus plane, independently of its lens origin.
            const double t = (cam.focus_dist - dot(offset, forward)) / distance;
            const vec3 target = offset + t * r.direction();
            const double x = dot(target, right) / cam.focus_dist;
            const double y = dot(target, up) / cam.focus_dist;
            origin_spread += offset.length_squared();
            const bool valid_origin = cam.defocus_angle == 0 ? offset.near_zero()
                : std::abs(dot(offset, forward)) < 1e-12 && offset.length() <= lens_radius + 1e-12;
            const double left = -half_width + 2 * half_width * i / cam.image_width;
            const double top = half_height - 2 * half_height * j / height;
            valid = valid && valid_origin && distance > 0
                && x >= left - 1e-12 && x <= left + 2 * half_width / cam.image_width + 1e-12
                && y <= top + 1e-12 && y >= top - 2 * half_height / height - 1e-12;
        }
        check(valid, "Ray origins and focus-plane pixel bounds must match camera geometry");
        if (cam.defocus_angle > 0)
            check(origin_spread > 0, "Nonzero aperture must sample varied lens origins");
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

    cam.defocus_angle = 10;
    cam.focus_dist = 3.4;
    verify(cam, vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1));
    cam.focus_dist = 1.7;
    verify(cam, vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1));
    cam.defocus_angle = 0;
    verify(cam, vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1));

    vec3 disk_mean;
    double radius_squared_sum = 0;
    bool inside_disk = true;
    for (int i = 0; i < 20000; ++i)
    {
        const vec3 p = random_in_unit_disk();
        disk_mean += p;
        radius_squared_sum += p.length_squared();
        inside_disk = inside_disk && p.z() == 0 && p.length_squared() < 1;
    }
    check(inside_disk && (disk_mean / 20000).length() < 0.02
          && std::abs(radius_squared_sum / 20000 - 0.5) < 0.02,
          "Lens sampling must be uniform over disk area, with mean squared radius 1/2");

    for (int scenario = 0; scenario < 14; ++scenario)
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
        if (scenario == 9) invalid.focus_dist = 0;
        if (scenario == 10) invalid.focus_dist = std::numeric_limits<double>::infinity();
        if (scenario == 11) invalid.defocus_angle = -1;
        if (scenario == 12) invalid.defocus_angle = 180;
        if (scenario == 13) invalid.defocus_angle = std::numeric_limits<double>::quiet_NaN();
        bool rejected = false;
        ray_capture scene;
        try { invalid.render(scene); }
        catch (const std::invalid_argument &) { rejected = true; }
        check(rejected && scene.rays.empty(), "Invalid settings must fail before ray generation");
    }
    return failures ? 1 : 0;
}
