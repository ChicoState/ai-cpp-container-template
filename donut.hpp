#pragma once

#include <string>

struct RenderConfig {
    int width;
    int height;
};

std::string render_frame(const RenderConfig& config, double angle_a, double angle_b);
