#include "donut.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr char kPalette[] = ".,-~:;=!*#$@";
constexpr int kPaletteSize = sizeof(kPalette) - 1;

std::size_t frame_size(const RenderConfig& config) {
    if (config.width <= 0 || config.height <= 0) {
        throw std::invalid_argument("frame dimensions must be positive");
    }

    const std::size_t width = static_cast<std::size_t>(config.width);
    const std::size_t height = static_cast<std::size_t>(config.height);
    if (height > std::numeric_limits<std::size_t>::max() / width) {
        throw std::invalid_argument("frame dimensions are too large");
    }

    return width * height;
}

}  // namespace

std::string render_frame(const RenderConfig& config, double angle_a, double angle_b) {
    const std::size_t size = frame_size(config);
    const std::size_t width = static_cast<std::size_t>(config.width);
    std::vector<double> depth_buffer(size, 0.0);
    std::string pixels(size, ' ');

    const double sin_a = std::sin(angle_a);
    const double cos_a = std::cos(angle_a);
    const double sin_b = std::sin(angle_b);
    const double cos_b = std::cos(angle_b);
    const double horizontal_scale = static_cast<double>(config.width) * 0.375;
    const double vertical_scale = static_cast<double>(config.height) * 0.68;

    for (double theta = 0.0; theta < 2.0 * kPi; theta += 0.07) {
        const double sin_theta = std::sin(theta);
        const double cos_theta = std::cos(theta);

        for (double phi = 0.0; phi < 2.0 * kPi; phi += 0.02) {
            const double sin_phi = std::sin(phi);
            const double cos_phi = std::cos(phi);
            const double circle_x = cos_phi + 2.0;
            const double inverse_depth = 1.0 /
                (sin_theta * circle_x * sin_a + sin_phi * cos_a + 5.0);
            const double rotated_x = sin_theta * circle_x * cos_a - sin_phi * sin_a;
            const int x = config.width / 2 + static_cast<int>(
                horizontal_scale * inverse_depth *
                (cos_theta * circle_x * cos_b - rotated_x * sin_b));
            const int y = config.height / 2 + static_cast<int>(
                vertical_scale * inverse_depth *
                (cos_theta * circle_x * sin_b + rotated_x * cos_b));
            const int luminance = static_cast<int>(8.0 *
                ((sin_phi * sin_a - sin_theta * circle_x * cos_a) * cos_b -
                 sin_theta * circle_x * sin_a - sin_phi * cos_a -
                 cos_theta * cos_phi * sin_b));

            if (x < 0 || x >= config.width || y < 0 || y >= config.height ||
                luminance < 0 || luminance >= kPaletteSize) {
                continue;
            }

            const std::size_t index = static_cast<std::size_t>(x) +
                width * static_cast<std::size_t>(y);
            if (inverse_depth > depth_buffer[index]) {
                depth_buffer[index] = inverse_depth;
                pixels[index] = kPalette[luminance];
            }
        }
    }

    std::string frame;
    frame.reserve(size + static_cast<std::size_t>(config.height));
    for (int y = 0; y < config.height; ++y) {
        const std::size_t row_start = width * static_cast<std::size_t>(y);
        frame.append(pixels, row_start, width);
        frame.push_back('\n');
    }

    return frame;
}
