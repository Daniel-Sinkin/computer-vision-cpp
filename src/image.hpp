#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>
#include <xtensor/views/xview.hpp>

class Image {
public:
    /// Load from file, optionally forcing number of channels (e.g. 4 === RGBA)
    explicit Image(const std::string &filename, int force_channels = 4);
    ~Image() = default;

    Image(const Image &) = delete;
    auto operator=(const Image &) -> Image & = delete;

    Image(Image &&) noexcept = default;
    auto operator=(Image &&) noexcept -> Image & = default;

    /// Raw pointer to the contiguous pixel data (row-major, channel-last)
    auto pixels() -> std::uint8_t * { return data_.data(); }
    auto pixels() const -> const std::uint8_t * { return data_.data(); }

    auto w() const -> int { return width_; }
    auto h() const -> int { return height_; }
    auto c() const -> int { return channels_; }

    /// Total size in bytes = w * h * c
    auto size_bytes() const -> std::size_t { return data_.size(); }
    /// Bytes per scanline = w * c
    auto row_stride() const -> std::size_t { return static_cast<std::size_t>(width_) * channels_; }

    /// Invert RGB channels in place (leaves alpha untouched if present)
    auto invert() -> void;
    xt::xarray<std::uint8_t> data_;

private:
    int width_ = 0;
    int height_ = 0;
    int channels_ = 0;
};