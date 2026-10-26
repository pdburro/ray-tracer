#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <memory>
#include <stdexcept>

image::image(const std::string &filename)
{
    int channels;
    std::unique_ptr<float, decltype(&stbi_image_free)> data(
        stbi_loadf(filename.c_str(), &width_, &height_, &channels, 3), stbi_image_free);
    if (!data)
        throw std::runtime_error("Cannot load texture '" + filename + "': " + stbi_failure_reason());
    const size_t count = size_t(width_) * size_t(height_) * 3;
    pixels_.assign(data.get(), data.get() + count);
}

color image::pixel(int x, int y) const
{
    x = std::clamp(x, 0, width_ - 1);
    y = std::clamp(y, 0, height_ - 1);
    const size_t index = 3 * (size_t(y) * size_t(width_) + size_t(x));
    return color(pixels_[index], pixels_[index + 1], pixels_[index + 2]);
}
