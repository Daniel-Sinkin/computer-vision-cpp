#pragma once
#include <memory>
#include <stdexcept>

#include "image.hpp"

class TextureImage {
public:
    TextureImage() = default;

    explicit TextureImage(std::unique_ptr<Image> img);

    TextureImage(TextureImage &&) noexcept = default;
    auto operator=(TextureImage &&) noexcept -> TextureImage & = default;

    TextureImage(const TextureImage &) = delete;
    auto operator=(const TextureImage &) -> TextureImage & = delete;

    ~TextureImage();

    auto upload() -> void;
    auto destroy() -> void;

    unsigned int texture{0};
    std::unique_ptr<Image> image;

private:
};