#include "texture_image.hpp"
#include <glad/glad.h>
#include <stdexcept>

TextureImage::TextureImage(std::unique_ptr<Image> img)
    : image(std::move(img)) {
    upload();
}

TextureImage::~TextureImage() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }
}

auto TextureImage::upload() -> void {
    if (!image) {
        throw std::runtime_error("TextureImage::upload: null image");
    }
    if (texture != 0) {
        throw std::runtime_error("TextureImage::upload: already uploaded");
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (auto err = glGetError(); err != GL_NO_ERROR) {
        texture = 0;
        throw std::runtime_error("TextureImage::upload: glGen/BindTexture failed with error code " + std::to_string(err));
    }

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        image->w(), image->h(), 0,
        GL_RGBA, GL_FLOAT,
        image->pixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

auto TextureImage::destroy() -> void {
    if (texture == 0) {
        throw std::runtime_error("TextureImage::destroy: no texture to delete");
    }
    glDeleteTextures(1, &texture);
    texture = 0;
}