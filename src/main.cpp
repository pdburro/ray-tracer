#include "color.h"
#include "ray.h"

#include <algorithm>
#include <iostream>

color ray_color(const ray &r)
{
    const vec3 direction = unit_vector(r.direction());
    const double blend = 0.5 * (direction.y() + 1.0);
    return (1.0 - blend) * color(1.0, 1.0, 1.0) + blend * color(0.5, 0.7, 1.0);
}

int main()
{
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = std::max(1, int(image_width / aspect_ratio));

    const point3 camera_center(0, 0, 0);
    const double focal_length = 1.0;
    const double viewport_height = 2.0;
    const double viewport_width = viewport_height * double(image_width) / image_height;
    const vec3 viewport_u(viewport_width, 0, 0);
    const vec3 viewport_v(0, -viewport_height, 0);
    const vec3 pixel_delta_u = viewport_u / image_width;
    const vec3 pixel_delta_v = viewport_v / image_height;
    const point3 upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    const point3 first_pixel = upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rRows left: " << image_height - j << "   " << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            const point3 pixel_center = first_pixel + i * pixel_delta_u + j * pixel_delta_v;
            const ray camera_ray(camera_center, pixel_center - camera_center);
            write_color(std::cout, ray_color(camera_ray));
        }
    }
    std::clog << "\rRender complete.       \n";
}
