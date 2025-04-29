#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class Image {
public:
    explicit Image(const std::string &filename, int force_channels = 4);
    ~Image();

    Image(const Image &) = delete;
    auto operator=(const Image &) -> Image & = delete;

    Image(Image &&) noexcept;
    auto operator=(Image &&) noexcept -> Image &;

    auto pixels() -> std::uint8_t *;
    auto pixels() const -> const std::uint8_t *;

    auto w() const -> int;
    auto h() const -> int;
    auto c() const -> int;

    auto size_bytes() const -> std::size_t;
    auto row_stride() const -> std::size_t;

private:
    std::uint8_t *data_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int channels_ = 0;
    std::size_t stride_bytes_ = 0;
};