#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

class hittable_list : public hittable
{
public:
    void clear() { objects.clear(); }
    void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

    bool hit(const ray &r, interval ray_t, hit_record &record) const override
    {
        hit_record candidate;
        bool hit_anything = false;
        double closest = ray_t.max;
        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest), candidate))
            {
                hit_anything = true;
                closest = candidate.t;
                record = candidate;
            }
        }
        return hit_anything;
    }

private:
    std::vector<std::shared_ptr<hittable>> objects;
};

#endif
