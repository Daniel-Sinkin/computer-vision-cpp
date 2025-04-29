// image.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "image.hpp"
#include <cstring>
#include <stdexcept>
#include <xtensor/containers/xadapt.hpp>
#include <xtensor/core/xmath.hpp>
#include <xtensor/generators/xbuilder.hpp>

#include "globals.hpp"

Image::Image(const std::string &filename, int force_channels) {
    stbi_set_flip_vertically_on_load(false);

    int w, h, c;
    auto *raw = stbi_load(filename.c_str(), &w, &h, &c, force_channels);
    if (raw == nullptr) {
        throw std::runtime_error(std::string("stbi_load failed: ") + stbi_failure_reason());
    }

    width_ = w;
    height_ = h;
    channels_ = (force_channels > 0 ? force_channels : c);

    std::vector<std::size_t> shape = {static_cast<std::size_t>(height_), static_cast<std::size_t>(width_), static_cast<std::size_t>(channels_)};
    std::vector<std::size_t> strides = {static_cast<std::size_t>(width_) * channels_, static_cast<std::size_t>(channels_), 1};

    auto adapted = xt::adapt(
        raw,
        static_cast<std::size_t>(width_ * height_ * channels_),
        xt::no_ownership(),
        shape,
        strides);
    xt::xtensor<float, 3> data_ = xt::cast<float>(adapted) / 255.0f;

    stbi_image_free(raw);
}

auto Image::invert() -> void {
    if (data_.size() == 0) return;

    auto rgb = xt::view(data_, xt::all(), xt::all(), xt::range(0, std::min(3, channels_)));
    rgb = 1.0f - rgb;
}

auto Image::apply_filter(xt::xtensor<float, 2> filter) -> Image {
    auto f_shape = filter.shape();
    size_t f_height = f_shape[0];
    size_t f_width = f_shape[1];

    size_t f_offset_y = f_height / 2;
    size_t f_offset_x = f_width / 2;

    size_t filtered_height = h() - 2 * f_offset_y;
    size_t filtered_width = w() - 2 * f_offset_x;
    size_t n_channels = get_num_channels();

    xt::xtensor<float, 3> filtered_image = xt::empty<float>({filtered_height, filtered_width, n_channels});
    if (n_channels == 4) { // No filter on alpha
        auto src_alpha = xt::view(data_, xt::range(f_offset_y, h() - f_offset_y),
            xt::range(f_offset_x, w() - f_offset_x),
            Constants::channel_alpha);

        auto dst_alpha = xt::view(filtered_image, xt::all(), xt::all(), Constants::channel_alpha);
        dst_alpha = src_alpha;
    }
    for (size_t c = 0; c < std::min<size_t>(n_channels, 3); ++c) {
        for (size_t y = 0; y < filtered_height; ++y) {
            for (size_t x = 0; x < filtered_width; ++x) {
                auto slice = xt::view(
                    data_,
                    xt::range(y, y + f_height),
                    xt::range(x, x + f_width),
                    c);
                filtered_image.at(y, x, c) = xt::sum(slice * filter)();
            }
        }
    }

    return Image(std::move(filtered_image));
}