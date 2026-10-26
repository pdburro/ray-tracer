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

// Section 7: camera setup, ray generation, and image output live together.
class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;
    double vfov = 90; // Vertical field of view in degrees.
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

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

    // Recalculate derived values so settings can change between renders.
    void initialize()
    {
        if (samples_per_pixel < 1)
            throw std::invalid_argument("samples_per_pixel must be positive");
        if (max_depth < 0)
            throw std::invalid_argument("max_depth must not be negative");
        if (image_width < 1 || !std::isfinite(aspect_ratio) || aspect_ratio <= 0
            || image_width / aspect_ratio >= std::numeric_limits<int>::max())
            throw std::invalid_argument("Invalid image dimensions or aspect ratio");
        if (!std::isfinite(vfov) || vfov <= 0 || vfov >= 180)
            throw std::invalid_argument("vfov must be between 0 and 180 degrees");
        const auto finite = [](const vec3 &p) {
            return std::isfinite(p.x()) && std::isfinite(p.y()) && std::isfinite(p.z());
        };
        if (!finite(lookfrom) || !finite(lookat) || !finite(vup))
            throw std::invalid_argument("Camera vectors must be finite");
        const vec3 view_back = lookfrom - lookat;
        const double focal_length = view_back.length();
        if (!std::isfinite(focal_length) || focal_length <= 1e-12)
            throw std::invalid_argument("Camera position and target must be distinct");
        const double up_length = vup.length();
        if (!std::isfinite(up_length) || up_length <= 1e-12)
            throw std::invalid_argument("Camera up vector must be nonzero");
        const vec3 w = view_back / focal_length;
        const vec3 right = cross(vup / up_length, w);
        if (right.length() <= 1e-12)
            throw std::invalid_argument("Camera up vector must not be parallel to view direction");
        const vec3 u = unit_vector(right);
        const vec3 v = cross(w, u);
        pixel_samples_scale = 1.0 / samples_per_pixel;
        image_height = std::max(1, int(image_width / aspect_ratio));

        camera_center = lookfrom;
        const double theta = vfov * std::acos(-1.0) / 180.0;
        const double viewport_height = 2 * focal_length * std::tan(theta / 2);
        const double viewport_width = viewport_height * double(image_width) / image_height;
        const vec3 viewport_u = viewport_width * u;
        const vec3 viewport_v = -viewport_height * v;
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        const point3 upper_left = camera_center - focal_length * w - viewport_u / 2 - viewport_v / 2;
        first_pixel = upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    vec3 sample_square() const
    {
        const double x = random_double() - 0.5;
        const double y = random_double() - 0.5;
        return vec3(x, y, 0);
    }

    ray get_ray(int i, int j) const
    {
        const vec3 offset = sample_square();
        const point3 pixel_sample = first_pixel
            + (i + offset.x()) * pixel_delta_u
            + (j + offset.y()) * pixel_delta_v;
        return ray(camera_center, pixel_sample - camera_center);
    }

    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        // A terminated path contributes no more light.
        if (depth <= 0) return color(0, 0, 0);

        hit_record record;
        // Ignore tiny intersections caused by rounding at a bounce's origin.
        if (world.hit(r, interval(0.001, std::numeric_limits<double>::infinity()), record))
        {
            ray scattered;
            color attenuation;
            if (record.mat->scatter(r, record, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        const vec3 direction = unit_vector(r.direction());
        const double blend = 0.5 * (direction.y() + 1.0);
        return (1.0 - blend) * color(1.0, 1.0, 1.0) + blend * color(0.5, 0.7, 1.0);
    }
};

#endif
