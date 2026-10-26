#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "image.h"
#include <algorithm>
#include <memory>
#include <stdexcept>

class texture
{
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
public:
    explicit solid_color(const color &albedo) : albedo_(albedo) {}
    color value(double, double, const point3 &) const override { return albedo_; }

private:
    color albedo_;
};

class checker_texture : public texture
{
public:
    checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
        : scale_(scale), even_(even), odd_(odd)
    {
        if (!std::isfinite(scale) || scale <= 0 || !even || !odd)
            throw std::invalid_argument("Checker needs a positive scale and two textures");
    }
    checker_texture(double scale, const color &even, const color &odd)
        : checker_texture(scale, std::make_shared<solid_color>(even), std::make_shared<solid_color>(odd)) {}

    color value(double u, double v, const point3 &p) const override
    {
        const double sum = std::floor(p.x() / scale_) + std::floor(p.y() / scale_)
                         + std::floor(p.z() / scale_);
        return std::fmod(sum, 2.0) == 0 ? even_->value(u, v, p) : odd_->value(u, v, p);
    }

private:
    double scale_;
    std::shared_ptr<texture> even_, odd_;
};

class image_texture : public texture
{
public:
    explicit image_texture(const std::string &filename) : image_(filename) {}

    color value(double u, double v, const point3 &) const override
    {
        u = std::clamp(u, 0.0, 1.0);
        v = 1.0 - std::clamp(v, 0.0, 1.0);
        return image_.pixel(int(u * image_.width()), int(v * image_.height()));
    }

private:
    image image_;
};

#endif
