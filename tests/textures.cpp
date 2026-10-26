#include "bvh.h"
#include "material.h"
#include "sphere.h"
#include <filesystem>
#include <iostream>

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    auto close = [](const color &a, const color &b) { return (a - b).length() < 1e-6; };
    const std::filesystem::path assets(TEST_ASSET_DIR);
    const color red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1);
    solid_color solid(red);
    check(close(solid.value(0.7, 0.3, point3(9, -1, 4)), red), "Solid texture must ignore coordinates");
    checker_texture checker(1, red, green);
    check(close(checker.value(0, 0, point3(0.2, 0.2, 0.2)), red)
          && close(checker.value(0, 0, point3(1.2, 0.2, 0.2)), green)
          && close(checker.value(0, 0, point3(-0.2, 0.2, 0.2)), green)
          && close(checker.value(0, 0, point3(-1.2, 0.2, 0.2)), red),
          "Checker parity must use floor for positive and negative positions");
    checker_texture scaled(0.5, red, green);
    check(close(scaled.value(0, 0, point3(0.7, 0, 0)), green), "Checker scale must control cell size");
    bool rejected_scale = false;
    try { checker_texture invalid(0, red, green); }
    catch (const std::invalid_argument &) { rejected_scale = true; }
    check(rejected_scale, "Reject zero checker scale");

    image png((assets / "test-colors.png").string());
    check(png.width() == 2 && png.height() == 2, "PNG dimensions must load");
    check(close(png.pixel(0, 0), red) && close(png.pixel(1, 0), green)
          && close(png.pixel(0, 1), blue), "PNG loader must retain RGB channel order");
    const double gray = std::pow(128.0 / 255, 2.2);
    check(close(png.pixel(1, 1), color(gray, gray, gray)), "LDR texture values must decode to linear light");
    image_texture tex((assets / "test-colors.png").string());
    check(close(tex.value(0, 1, point3()), red) && close(tex.value(1, 1, point3()), green)
          && close(tex.value(0, 0, point3()), blue), "UV sampling must flip V and handle endpoints safely");
    check(close(tex.value(-1, 2, point3()), red)
          && close(tex.value(2, -1, point3()), color(gray, gray, gray)), "UV coordinates must clamp at image edges");
    image bmp((assets / "test-color.bmp").string());
    check(close(bmp.pixel(0, 0), color(1, 0, 1)), "BMP loading must work alongside PNG");
    image jpg((assets / "earthmap.jpg").string());
    check(jpg.width() > 100 && jpg.height() > 100, "Earth JPEG must decode successfully");
    bool rejected_image = false;
    try { image missing((assets / "not-a-texture.jpg").string()); }
    catch (const std::runtime_error &) { rejected_image = true; }
    check(rejected_image, "Missing image must report a load failure");
    rejected_image = false;
    try { image invalid((assets / "README.md").string()); }
    catch (const std::runtime_error &) { rejected_image = true; }
    check(rejected_image, "Unsupported or corrupt image must report a load failure");

    auto matte = std::make_shared<lambertian>(red);
    sphere ball(point3(), 1, matte);
    struct uv_case { vec3 normal; double u, v; };
    const uv_case cases[] = {
        {vec3(1, 0, 0), 0.5, 0.5}, {vec3(0, 0, 1), 0.25, 0.5},
        {vec3(0, 0, -1), 0.75, 0.5}, {vec3(0, 1, 0), 0.5, 1},
        {vec3(0, -1, 0), 0.5, 0}
    };
    for (const auto &expected : cases)
    {
        hit_record hit;
        const bool found = ball.hit(ray(3 * expected.normal, -expected.normal), interval(0.001, 100), hit);
        check(found && std::abs(hit.v - expected.v) < 1e-12
              && (std::abs(expected.normal.y()) == 1 || std::abs(hit.u - expected.u) < 1e-12),
              "Sphere UVs must match the textbook axis directions and poles");
    }
    hit_record seam;
    check(ball.hit(ray(point3(-3, 0, 0), vec3(1, 0, 0)), interval(0.001, 100), seam)
          && (std::abs(seam.u) < 1e-12 || std::abs(seam.u - 1) < 1e-12), "Sphere seam must map to an image edge");
    hit_record inside;
    check(ball.hit(ray(point3(), vec3(1, 0, 0)), interval(0.001, 100), inside)
          && !inside.front_face && std::abs(inside.u - 0.5) < 1e-12,
          "Back-face UVs must use outward geometry, not the flipped shading normal");

    auto mapped = std::make_shared<image_texture>((assets / "test-colors.png").string());
    lambertian textured(mapped);
    hit_record record;
    record.p = point3(4, 5, 6);
    record.normal = vec3(0, 1, 0);
    record.u = 0; record.v = 1;
    color attenuation;
    ray scattered;
    check(textured.scatter(ray(point3(), vec3(0, -1, 0), 0.3), record, attenuation, scattered)
          && close(attenuation, red) && scattered.time() == 0.3,
          "Lambertian must sample image UVs and preserve path time");

    hittable_list world;
    world.add(std::make_shared<sphere>(point3(2, 0, 0), point3(2, 1, 0), 2, matte));
    bvh_node tree(world);
    const ray r(point3(6, 0.5, 0), vec3(-1, 0, 0), 0.5);
    hit_record direct, accelerated;
    check(world.hit(r, interval(0.001, 100), direct) && tree.hit(r, interval(0.001, 100), accelerated)
          && std::abs(direct.u - 0.5) < 1e-12 && direct.u == accelerated.u
          && direct.v == accelerated.v, "Moving/transformed sphere UVs must survive BVH traversal");
    return failures ? 1 : 0;
}
