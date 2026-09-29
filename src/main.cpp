#include "color.h"

#include <iostream>
#include <cmath>
#include <algorithm>

int main() {

    // Image

    int image_width = 256;
    int image_height = 256;

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rRows left: " << image_height - j << "   " << std::flush;
        for (int i = 0; i < image_width; i++) {
            double hue = 5.0 * double(i) / (image_width - 1);

            auto channel = [hue](double offset) {
                double k = std::fmod(hue + offset, 6.0);
                return std::clamp(std::abs(k - 3.0) - 1.0, 0.0, 1.0);
            };

            auto r = channel(0.0);
            auto g = channel(4.0);
            auto b = channel(2.0);

            color pixel_color(r, g, b);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rRender complete.       \n";
}
