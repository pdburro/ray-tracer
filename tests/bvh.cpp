#include "bvh.h"
#include "material.h"
#include "sphere.h"
#include <iostream>

class counted_object : public hittable
{
public:
    counted_object(std::shared_ptr<hittable> object, int &count) : object_(object), count_(count) {}
    bool hit(const ray &r, interval range, hit_record &record) const override
    {
        ++count_;
        return object_->hit(r, range, record);
    }
    aabb bounding_box() const override { return object_->bounding_box(); }

private:
    std::shared_ptr<hittable> object_;
    int &count_;
};

int main()
{
    int failures = 0;
    auto check = [&](bool condition, const char *message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    const interval forward(0.001, 1000);
    aabb box(point3(1, 1, 1), point3(-1, -1, -1));
    check(box.hit(ray(point3(0, 0, 3), vec3(0, 0, -1)), forward), "Negative direction must reach box");
    check(box.hit(ray(point3(), vec3(1, 0, 0)), forward), "Ray inside box must reach its exit");
    check(box.hit(ray(point3(1, 0, 3), vec3(-0.0, 0, -1)), forward),
          "Parallel ray on boundary must not be lost");
    check(!box.hit(ray(point3(2, 0, 3), vec3(0, 0, -1)), forward), "Parallel ray outside slab must miss");
    check(!box.hit(ray(point3(0, 0, 3), vec3(0, 0, 1)), forward), "Box behind ray must miss");
    check(!box.hit(ray(point3(0, 0, 3), vec3(0, 0, -1)), interval(0, 1)), "Respect upper ray bound");
    check(!aabb().hit(ray(point3(), vec3(1, 0, 0)), forward), "Empty box must miss");
    aabb flat(point3(-1, -1, 0), point3(1, 1, 0));
    check(flat.z.size() >= 0.0001, "Flat boxes must have nonzero thickness");

    auto matte = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    sphere moving(point3(-2, 0, 0), point3(2, 3, -1), 0.5, matte);
    const aabb swept = moving.bounding_box();
    bool enclosed = true;
    for (double time : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const point3 center = point3(-2, 0, 0) + time * vec3(4, 3, -1);
        for (int axis = 0; axis < 3; ++axis)
            enclosed = enclosed && swept.axis_interval(axis).contains(center[axis] - 0.5)
                                && swept.axis_interval(axis).contains(center[axis] + 0.5);
    }
    check(enclosed, "Moving bounds must contain the sphere over the entire shutter interval");

    hittable_list world;
    int calls = 0;
    for (int x = -4; x <= 4; ++x)
        for (int z = -4; z <= 4; ++z)
        {
            auto surface = std::make_shared<lambertian>(color::random());
            auto ball = std::make_shared<sphere>(point3(x, 0, z), point3(x, 0.5, z), 0.3, surface);
            world.add(std::make_shared<counted_object>(ball, calls));
        }
    bvh_node tree(world);
    int linear_calls = 0, bvh_calls = 0, hits = 0;
    bool equivalent = true;
    for (int i = 0; i < 5000; ++i)
    {
        const point3 origin = vec3::random(-6, 6);
        const vec3 target(random_double(-4, 4), random_double(0, 0.5), random_double(-4, 4));
        const ray r(origin, target - origin, random_double());
        hit_record linear, accelerated;
        calls = 0;
        const bool a = world.hit(r, forward, linear);
        linear_calls += calls;
        calls = 0;
        const bool b = tree.hit(r, forward, accelerated);
        bvh_calls += calls;
        equivalent = equivalent && a == b;
        if (a && b)
        {
            ++hits;
            equivalent = equivalent && std::abs(linear.t - accelerated.t) < 1e-10
                && (linear.p - accelerated.p).near_zero()
                && (linear.normal - accelerated.normal).near_zero()
                && linear.front_face == accelerated.front_face && linear.mat == accelerated.mat;
        }
    }
    check(equivalent && hits > 100, "BVH must preserve closest hit, point, normal, face, and material");
    check(bvh_calls < linear_calls / 4, "BVH must substantially reduce primitive intersection calls");
    std::cout << "Primitive tests: linear=" << linear_calls << ", BVH=" << bvh_calls << '\n';

    hittable_list single;
    single.add(std::make_shared<counted_object>(std::make_shared<sphere>(point3(0, 0, -1), 0.5, matte), calls));
    bvh_node leaf(single);
    calls = 0;
    hit_record record;
    check(leaf.hit(ray(point3(0.5, 0, 0), vec3(0, 0, -1)), forward, record) && calls == 1,
          "Single leaf must retain tangent hit and test its object once");
    world.clear();
    check(!world.bounding_box().hit(ray(point3(), vec3(1, 0, 0)), forward), "Clearing list must reset bounds");
    bool rejected = false;
    try { bvh_node empty(world); }
    catch (const std::invalid_argument &) { rejected = true; }
    check(rejected, "Empty BVH must be rejected rather than recurse indefinitely");
    return failures ? 1 : 0;
}
