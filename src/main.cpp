#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"

// Book 1, Section 14: the final random-sphere scene.
int main()
{
    hittable_list world;
    auto ground = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    // Section 14: scatter small spheres across grid cells and choose their materials.
    int diffuse_count = 0, metal_count = 0, glass_count = 0;
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            const double choice = random_double();
            const double x = a + 0.9 * random_double();
            const double z = b + 0.9 * random_double();
            const point3 center(x, 0.2, z);
            // Leave room around the large metal sphere, as in the book.
            if ((center - point3(4, 0.2, 0)).length() <= 0.9) continue;

            std::shared_ptr<material> surface;
            // Section 14: 80% diffuse, 15% metal, 5% glass.
            if (choice < 0.8)
            {
                const color first = color::random();
                const color second = color::random();
                surface = std::make_shared<lambertian>(first * second);
                ++diffuse_count;
            }
            else if (choice < 0.95)
            {
                const color albedo = color::random(0.5, 1);
                const double fuzz = random_double(0, 0.5);
                surface = std::make_shared<metal>(albedo, fuzz);
                ++metal_count;
            }
            else
            {
                surface = std::make_shared<dielectric>(1.5);
                ++glass_count;
            }
            world.add(std::make_shared<sphere>(center, 0.2, surface));
        }
    }

    // Section 14: large glass, diffuse, and polished metal spheres.
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1,
        std::make_shared<dielectric>(1.5)));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1,
        std::make_shared<lambertian>(color(0.4, 0.2, 0.1))));
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1,
        std::make_shared<metal>(color(0.7, 0.6, 0.5), 0)));

    // Project diagnostic: keep scene counts out of the PPM stream.
    std::clog << "Small spheres: " << diffuse_count << " diffuse, "
              << metal_count << " metal, " << glass_count << " glass; "
              << diffuse_count + metal_count + glass_count + 4 << " total objects.\n";

    // Sections 7-9: image size, antialiasing, and path depth.
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    // Development settings. Book's high-quality settings: width 1200, samples 500.
    cam.image_width = 400;
    cam.samples_per_pixel = 25;
    cam.max_depth = 50;
    // Sections 12 and 14: final viewpoint and vertical field of view.
    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    // Sections 13-14: a small lens aperture keeps the focus blur subtle.
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10;
    cam.render(world);
}
