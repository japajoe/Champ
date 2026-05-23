#pragma once

#include "Color.hpp"
#include "Texture.hpp"
#include <cstdint>
#include <cstdlib>

namespace Champ
{
    class Image;

    class Texture2D
    {
    public:
        Texture2D();
        Texture2D(uint32_t id, uint32_t width, uint32_t height);
        Texture2D(const Texture2D &other) = delete;
        Texture2D(Texture2D &&other) noexcept;
        Texture2D &operator=(const Texture2D &other) = delete;
        Texture2D &operator=(Texture2D &&other) noexcept;
        void Generate(const Image *img, const TextureSettings *settings = nullptr);
        void Generate(uint32_t width, uint32_t height, const Color &color);
        void Generate(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels, const TextureSettings *settings = nullptr);
        void Destroy();
        void Bind(uint32_t unit);
        void Unbind();
        void SetMaxAnisotropyLevel(float level);
        uint32_t GetId() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
    };
}