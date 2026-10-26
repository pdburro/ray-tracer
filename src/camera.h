#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
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
                    pixel_color += ray_color(get_ray(i, j), world);
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
        pixel_samples_scale = 1.0 / samples_per_pixel;
        image_height = std::max(1, int(image_width / aspect_ratio));

        camera_center = point3(0, 0, 0);
        const double focal_length = 1.0;
        const double viewport_height = 2.0;
        const double viewport_width = viewport_height * double(image_width) / image_height;
        const vec3 viewport_u(viewport_width, 0, 0);
        const vec3 viewport_v(0, -viewport_height, 0);
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        const point3 upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
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

    color ray_color(const ray &r, const hittable &world) const
    {
        hit_record record;
        if (world.hit(r, interval(0, std::numeric_limits<double>::infinity()), record))
        {
            return 0.5 * (record.normal + color(1, 1, 1));
        }

        const vec3 direction = unit_vector(r.direction());
        const double blend = 0.5 * (direction.y() + 1.0);
        return (1.0 - blend) * color(1.0, 1.0, 1.0) + blend * color(0.5, 0.7, 1.0);
    }
};

#endif
