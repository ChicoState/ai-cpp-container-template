#include "donut.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

void expect_invalid_dimensions(int width, int height) {
    try {
        render_frame(RenderConfig{width, height}, 0.0, 0.0);
        expect(false, "invalid dimensions must throw");
    } catch (const std::invalid_argument&) {
    }
}

void test_frame_shape_and_palette() {
    constexpr int width = 40;
    constexpr int height = 16;
    const std::string frame = render_frame(RenderConfig{width, height}, 0.0, 0.0);
    const std::string allowed = " ., -~:;=!*#$@\n";

    int row_width = 0;
    int rows = 0;
    bool has_glyph = false;
    for (const char character : frame) {
        expect(allowed.find(character) != std::string::npos,
               "frame contains only palette characters, spaces, and newlines");
        if (character == '\n') {
            expect(row_width == width, "each frame row has the configured width");
            row_width = 0;
            ++rows;
        } else {
            has_glyph = has_glyph || character != ' ';
            ++row_width;
        }
    }

    expect(rows == height, "frame has the configured height");
    expect(has_glyph, "frame contains visible donut glyphs");
}

void test_rotation_changes_frame_without_changing_shape() {
    const RenderConfig config{40, 16};
    const std::string baseline = render_frame(config, 0.0, 0.0);
    const std::string rotated = render_frame(config, 0.6, 0.8);

    expect(baseline.size() == rotated.size(), "rotation preserves frame dimensions");
    expect(baseline != rotated, "rotation changes frame content");
}

void test_invalid_dimensions_are_rejected() {
    expect_invalid_dimensions(0, 16);
    expect_invalid_dimensions(40, 0);
    expect_invalid_dimensions(-1, 16);
    expect_invalid_dimensions(40, -1);
}

}  // namespace

int main() {
    test_frame_shape_and_palette();
    test_rotation_changes_frame_without_changing_shape();
    test_invalid_dimensions_are_rejected();

    if (failures == 0) {
        std::cout << "All donut renderer tests passed.\n";
        return 0;
    }

    return 1;
}
