#pragma once

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

#include <xtensor/containers/xarray.hpp>
#include <xtensor/containers/xtensor.hpp>
#include <xtensor/core/xmath.hpp>
#include <xtensor/io/xio.hpp>
#include <xtensor/views/xview.hpp>

constexpr size_t ostream_precision = 4;

class Image {
public:
    explicit Image(const std::string &filename, int force_channels = 4);
    explicit Image(xt::xtensor<float, 3> img)
        : data_(std::move(img)),
          height_(static_cast<int>(data_.shape()[0])),
          width_(static_cast<int>(data_.shape()[1])),
          channels_(static_cast<int>(data_.shape()[2])) {}

    ~Image() = default;

    Image(const Image &) = delete;
    auto operator=(const Image &) -> Image & = delete;

    Image(Image &&) noexcept = default;
    auto operator=(Image &&) noexcept -> Image & = default;

    auto pixels() -> float * { return data_.data(); }
    auto pixels() const -> const float * { return data_.data(); }

    auto to_uint8_copy() const -> xt::xtensor<std::uint8_t, 3> {
        return xt::cast<std::uint8_t>(xt::clip(data_, 0.0f, 1.0f) * 255.0f);
    }

    auto h() const -> int { return height_; }
    auto w() const -> int { return width_; }
    auto get_num_channels() const -> int { return channels_; }

    auto size_bytes() const -> std::size_t { return data_.size(); }
    auto row_stride() const -> std::size_t { return static_cast<std::size_t>(width_) * channels_; }

    auto invert() -> void;

    auto apply_filter(xt::xtensor<float, 2> filter) -> Image;

    xt::xtensor<float, 3> data_;

private:
    int height_ = 0;
    int width_ = 0;
    int channels_ = 0;
};

inline auto operator<<(std::ostream &os, const Image &img) -> std::ostream & {
    static const char *channel_names[] = {"Red", "Green", "Blue", "Alpha"};

    for (int ch = 0; ch < img.get_num_channels(); ++ch) {
        os << (ch < 4 ? channel_names[ch] : ("Channel " + std::to_string(ch))) << "\n";

        auto channel = xt::view(img.data_, xt::all(), xt::all(), ch);

        std::size_t count = 0;
        for (const auto &v : channel) {
            std::ostringstream formatted;
            formatted << std::fixed << std::setprecision(ostream_precision) << v;
            std::string str = formatted.str();

            // Remove leading '0' if appropriate
            if (str.length() >= 2 && str[0] == '0' && str[1] == '.') {
                str.erase(0, 1);
            }

            os << str;

            ++count;
            if (count % img.w() == 0) {
                os << "\n";
            } else {
                os << " ";
            }
        }
        os << "\n";
    }

    return os;
}