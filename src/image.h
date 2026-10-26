#ifndef IMAGE_H
#define IMAGE_H

#include "color.h"
#include <string>
#include <vector>

class image
{
public:
    explicit image(const std::string &filename);
    int width() const { return width_; }
    int height() const { return height_; }
    color pixel(int x, int y) const;

private:
    int width_ = 0, height_ = 0;
    std::vector<float> pixels_;
};

#endif
