#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"

int main()
{
    hittable_list world;
    auto ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto left = std::make_shared<dielectric>(1.5);
    auto bubble = std::make_shared<dielectric>(1.0 / 1.5);
    auto right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100, ground));
    world.add(std::make_shared<sphere>(point3(0, 0, -1.2), 0.5, center));
    world.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.5, left));
    // Air cavity inside glass: both boundaries use ordinary positive radii.
    world.add(std::make_shared<sphere>(point3(-1, 0, -1), 0.4, bubble));
    world.add(std::make_shared<sphere>(point3(1, 0, -1), 0.5, right));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat = point3(0, 0, -1);
    cam.vup = vec3(0, 1, 0);
    cam.render(world);
}
