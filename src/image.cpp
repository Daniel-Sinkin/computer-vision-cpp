// image.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "image.hpp"
#include <cstring>
#include <stdexcept>
#include <xtensor/generators/xbuilder.hpp>

Image::Image(const std::string &filename, int force_channels) {
    stbi_set_flip_vertically_on_load(false);

    int w, h, c;
    auto *raw = stbi_load(filename.c_str(), &w, &h, &c, force_channels);
    if (!raw) {
        throw std::runtime_error(std::string("stbi_load failed: ") + stbi_failure_reason());
    }

    width_ = w;
    height_ = h;
    channels_ = (force_channels > 0 ? force_channels : c);

    data_ = xt::zeros<std::uint8_t>({height_, width_, channels_});
    std::memcpy(data_.data(), raw, static_cast<std::size_t>(width_) * height_ * channels_);

    stbi_image_free(raw);
}

auto Image::invert() -> void {
    if (data_.size() == 0) return;

    auto rgb = xt::view(data_, xt::all(), xt::all(), xt::range(0, std::min(3, channels_)));
    rgb = 255 - rgb;
}