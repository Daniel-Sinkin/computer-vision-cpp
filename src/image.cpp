#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.hpp"

#include <iostream>
#include <stdexcept>

Image::Image(const std::string &filename, int force_channels) {
    stbi_set_flip_vertically_on_load(false);

    data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, force_channels);
    if (!data_) {
        throw std::runtime_error(std::string("stbi_load failed: ") + stbi_failure_reason());
    }
    if (force_channels > 0) {
        channels_ = force_channels;
    }
    stride_bytes_ = static_cast<std::size_t>(width_) * channels_;
}

Image::~Image() { stbi_image_free(data_); }

auto Image::operator=(Image &&other) noexcept -> Image & {
    if (this != &other) {
        stbi_image_free(data_);

        data_ = other.data_;
        width_ = other.width_;
        height_ = other.height_;
        channels_ = other.channels_;
        stride_bytes_ = other.stride_bytes_;

        other.data_ = nullptr;
        other.width_ = 0;
        other.height_ = 0;
        other.channels_ = 0;
        other.stride_bytes_ = 0;
    }
    return *this;
}

auto Image::invert() -> void {
    if (!data_) {
        std::cerr << "Tried to invert but no image data availiabe!\n";
        return;
    }
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            std::uint8_t *pixel = data_ + y * stride_bytes_ + x * channels_;

            for (int c = 0; c < std::min(3, channels_); ++c) {
                pixel[c] = 255 - pixel[c];
            }
        }
    }
}

Image::Image(Image &&other) noexcept
    : data_(other.data_), width_(other.width_), height_(other.height_), channels_(other.channels_), stride_bytes_(other.stride_bytes_) {
    other.data_ = nullptr;
    other.width_ = 0;
    other.height_ = 0;
    other.channels_ = 0;
    other.stride_bytes_ = 0;
}
auto Image::pixels() -> std::uint8_t * { return data_; }
auto Image::pixels() const -> const std::uint8_t * { return data_; }
auto Image::w() const -> int { return width_; }
auto Image::h() const -> int { return height_; }
auto Image::c() const -> int { return channels_; }
auto Image::size_bytes() const -> std::size_t { return static_cast<std::size_t>(width_) * height_ * channels_; }
auto Image::row_stride() const -> std::size_t { return stride_bytes_; }