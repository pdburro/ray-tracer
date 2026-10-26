#ifndef BVH_H
#define BVH_H

#include "hittable_list.h"
#include <algorithm>
#include <stdexcept>

class bvh_node : public hittable
{
public:
    explicit bvh_node(const hittable_list &world)
    {
        auto objects = world.items();
        if (objects.empty()) throw std::invalid_argument("Cannot build an empty BVH");
        build(objects, 0, objects.size());
    }

    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        if (!box_.hit(r, ray_t)) return false;
        const bool hit_left = left_->hit(r, ray_t, record);
        if (!right_) return hit_left;
        const bool hit_right = right_->hit(r, interval(ray_t.min, hit_left ? record.t : ray_t.max), record);
        return hit_left || hit_right;
    }
    aabb bounding_box() const override { return box_; }

private:
    std::shared_ptr<hittable> left_, right_;
    aabb box_;

    bvh_node(std::vector<std::shared_ptr<hittable>> &objects, size_t start, size_t end)
    {
        build(objects, start, end);
    }

    void build(std::vector<std::shared_ptr<hittable>> &objects, size_t start, size_t end)
    {
        for (size_t i = start; i < end; ++i) box_ = aabb(box_, objects[i]->bounding_box());
        if (end - start == 1)
        {
            left_ = objects[start];
            return;
        }
        const int axis = box_.longest_axis();
        std::stable_sort(objects.begin() + start, objects.begin() + end,
            [axis](const auto &a, const auto &b) {
                return a->bounding_box().axis_interval(axis).min
                     < b->bounding_box().axis_interval(axis).min;
            });
        const size_t middle = start + (end - start) / 2;
        left_ = std::shared_ptr<hittable>(new bvh_node(objects, start, middle));
        right_ = std::shared_ptr<hittable>(new bvh_node(objects, middle, end));
    }
};

#endif
