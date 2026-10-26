#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "random.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

// Book 1, Section 7: the camera owns ray generation and image output.
class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10; // Section 8: rays averaged into each pixel.
    int max_depth = 10; // Section 9: stop paths after this many ray evaluations.

    // Section 12: position, viewing direction, roll, and vertical FOV.
    double vfov = 90; // Degrees.
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    // Section 13: lens size and the distance to the plane of sharp focus.
    double defocus_angle = 0; // Full cone angle in degrees; zero is a pinhole.
    double focus_dist = 10; // Distance along the view axis to the focus plane.

    // Sections 2 and 7: write a PPM image; Section 8: average the pixel samples.
    void render(const hittable &world)
    {
        initialize();
        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rRows left: " << image_height - j << "   " << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample)
                    pixel_color += ray_color(get_ray(i, j), max_depth, world);
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rRender complete.       \n";
    }

private:
    int image_height = 0;
    double pixel_samples_scale = 1;
    point3 camera_center;
    point3 first_pixel;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    // Sections 7, 12, and 13: rebuild the camera geometry for the current settings.
    void initialize()
    {
        // Project checks: reject settings that would make the geometry undefined.
        if (samples_per_pixel < 1)
            throw std::invalid_argument("samples_per_pixel must be positive");
        if (max_depth < 0)
            throw std::invalid_argument("max_depth must not be negative");
        if (image_width < 1 || !std::isfinite(aspect_ratio) || aspect_ratio <= 0
            || image_width / aspect_ratio >= std::numeric_limits<int>::max())
            throw std::invalid_argument("Invalid image dimensions or aspect ratio");
        if (!std::isfinite(vfov) || vfov <= 0 || vfov >= 180)
            throw std::invalid_argument("vfov must be between 0 and 180 degrees");
        if (!std::isfinite(focus_dist) || focus_dist <= 0)
            throw std::invalid_argument("focus_dist must be finite and positive");
        if (!std::isfinite(defocus_angle) || defocus_angle < 0 || defocus_angle >= 180)
            throw std::invalid_argument("defocus_angle must be in [0, 180) degrees");
        const auto finite = [](const vec3 &p) {
            return std::isfinite(p.x()) && std::isfinite(p.y()) && std::isfinite(p.z());
        };
        if (!finite(lookfrom) || !finite(lookat) || !finite(vup))
            throw std::invalid_argument("Camera vectors must be finite");
        const vec3 view_back = lookfrom - lookat;
        const double target_distance = view_back.length();
        if (!std::isfinite(target_distance) || target_distance <= 1e-12)
            throw std::invalid_argument("Camera position and target must be distinct");
        const double up_length = vup.length();
        if (!std::isfinite(up_length) || up_length <= 1e-12)
            throw std::invalid_argument("Camera up vector must be nonzero");
        // Section 12: w points backward, u right, and v up.
        const vec3 w = view_back / target_distance;
        const vec3 right = cross(vup / up_length, w);
        if (right.length() <= 1e-12)
            throw std::invalid_argument("Camera up vector must not be parallel to view direction");
        const vec3 u = unit_vector(right);
        const vec3 v = cross(w, u);
        // Section 8: average in floating point before encoding the color.
        pixel_samples_scale = 1.0 / samples_per_pixel;
        image_height = std::max(1, int(image_width / aspect_ratio));

        // Sections 12-13: the pixel grid lies on the focus plane.
        camera_center = lookfrom;
        const double theta = vfov * std::acos(-1.0) / 180.0;
        const double viewport_height = 2 * focus_dist * std::tan(theta / 2);
        const double viewport_width = viewport_height * double(image_width) / image_height;
        const vec3 viewport_u = viewport_width * u;
        const vec3 viewport_v = -viewport_height * v; // Image rows run downward.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        const point3 upper_left = camera_center - focus_dist * w - viewport_u / 2 - viewport_v / 2;
        first_pixel = upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        // Section 13: orient the lens disk using the camera right/up axes.
        const double defocus_radius = focus_dist * std::tan(defocus_angle * std::acos(-1.0) / 360);
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }

    // Section 8: jitter within half a pixel of its center.
    vec3 sample_square() const
    {
        const double x = random_double() - 0.5;
        const double y = random_double() - 0.5;
        return vec3(x, y, 0);
    }

    // Section 8 selects the pixel sample; Section 13 selects the lens origin.
    ray get_ray(int i, int j) const
    {
        const vec3 offset = sample_square();
        const point3 pixel_sample = first_pixel
            + (i + offset.x()) * pixel_delta_u
            + (j + offset.y()) * pixel_delta_v;
        const point3 origin = defocus_angle == 0 ? camera_center : defocus_disk_sample();
        return ray(origin, pixel_sample - origin);
    }

    // Section 13: map a 2D unit-disk sample onto the camera lens.
    point3 defocus_disk_sample() const
    {
        const vec3 p = random_in_unit_disk();
        return camera_center + p.x() * defocus_disk_u + p.y() * defocus_disk_v;
    }

    // Sections 9-11: follow scattered rays and accumulate material attenuation.
    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        // Section 9: discard the remaining contribution at the bounce limit.
        if (depth <= 0) return color(0, 0, 0);

        hit_record record;
        // Section 9: ignore near-zero hits to avoid shadow acne after a bounce.
        if (world.hit(r, interval(0.001, std::numeric_limits<double>::infinity()), record))
        {
            ray scattered;
            color attenuation;
            // Section 10: the material chooses a new ray or absorbs the path.
            if (record.mat->scatter(r, record, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        // Section 4: a ray that misses the scene sees the sky gradient.
        const vec3 direction = unit_vector(r.direction());
        const double blend = 0.5 * (direction.y() + 1.0);
        return (1.0 - blend) * color(1.0, 1.0, 1.0) + blend * color(0.5, 0.7, 1.0);
    }
};

#endif
