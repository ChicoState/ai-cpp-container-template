#include "donut.hpp"

#include <charconv>
#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <thread>

namespace {

constexpr RenderConfig kDefaultRenderConfig{80, 22};
constexpr int kDefaultDelayMilliseconds = 30;
constexpr double kAngleAStep = 0.04;
constexpr double kAngleBStep = 0.02;

struct Options {
    std::optional<int> frames;
    int delay_milliseconds = kDefaultDelayMilliseconds;
};

enum class ParseResult {
    ok,
    help,
    error,
};

void print_usage(std::ostream& output) {
    output << "Usage: app [--frames N] [--delay-ms N]\n"
           << "\n"
           << "Render a spinning ASCII donut. Run without --frames to animate until Ctrl-C.\n"
           << "\n"
           << "Options:\n"
           << "  --frames N    Render exactly N frames (N must be non-negative).\n"
           << "  --delay-ms N  Wait N milliseconds between frames (N must be non-negative).\n"
           << "  --help        Show this help message.\n";
}

bool parse_non_negative_int(std::string_view text, int& value) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    const std::from_chars_result result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end && value >= 0;
}

ParseResult parse_options(int argc, char* argv[], Options& options) {
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument(argv[index]);
        if (argument == "--help") {
            print_usage(std::cout);
            return ParseResult::help;
        }

        if (argument != "--frames" && argument != "--delay-ms") {
            std::cerr << "Unknown option: " << argument << '\n';
            return ParseResult::error;
        }

        if (index + 1 >= argc) {
            std::cerr << "Missing value for " << argument << '\n';
            return ParseResult::error;
        }

        int value = 0;
        if (!parse_non_negative_int(argv[++index], value)) {
            std::cerr << "Expected a non-negative integer for " << argument << '\n';
            return ParseResult::error;
        }

        if (argument == "--frames") {
            options.frames = value;
        } else {
            options.delay_milliseconds = value;
        }
    }

    return ParseResult::ok;
}

}  // namespace

int main(int argc, char* argv[]) {
    Options options;
    const ParseResult parse_result = parse_options(argc, argv, options);
    if (parse_result == ParseResult::help) {
        return 0;
    }
    if (parse_result == ParseResult::error) {
        return 1;
    }

    std::cout << "\x1b[2J";
    double angle_a = 0.0;
    double angle_b = 0.0;
    int rendered_frames = 0;

    while (!options.frames || rendered_frames < *options.frames) {
        std::cout << "\x1b[H" << render_frame(kDefaultRenderConfig, angle_a, angle_b)
                  << std::flush;
        angle_a += kAngleAStep;
        angle_b += kAngleBStep;
        ++rendered_frames;

        if (options.delay_milliseconds > 0) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(options.delay_milliseconds));
        }
    }

    std::cout << '\n';
    return 0;
}
