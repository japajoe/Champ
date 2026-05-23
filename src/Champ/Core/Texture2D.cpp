#include "Texture2D.hpp"
#include "OpenGL.hpp"
#include "Image.hpp"
#include <cstring>
#include <utility>
#include <stdexcept>

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

namespace Champ
{
    Texture2D::Texture2D()
    {
        id = 0;
        width = 0;
        height = 0;
    }

    Texture2D::Texture2D(uint32_t id, uint32_t width, uint32_t height)
    {
        this->id = id;
        this->width = width;
        this->height = height;
    }

    Texture2D::Texture2D(Texture2D &&other) noexcept
    {
        id = std::exchange(other.id, 0);
        width = std::exchange(other.width, 0);
        height = std::exchange(other.height, 0);
    }

    Texture2D &Texture2D::operator=(Texture2D &&other) noexcept
    {
        if (this != &other)
        {
            id = std::exchange(other.id, 0);
            width = std::exchange(other.width, 0);
            height = std::exchange(other.height, 0);
        }
        return *this;
    }

    void Texture2D::Generate(const Image *img, const TextureSettings *settings)
    {
        Generate(img->GetData(), img->GetDataSize(), img->GetWidth(), img->GetHeight(), img->GetChannels(), settings);
    }

    void Texture2D::Generate(uint32_t width, uint32_t height, const Color &color)
    {
        if (width == 0 || height == 0)
            throw std::invalid_argument("Failed to generate texture:  width and height must be greater than 0");

        uint32_t channels = 4;
        uint32_t size = width * height * channels;
        uint8_t *data = new uint8_t[size];

        if (data)
        {
            auto clamp = [](float value, float min, float max) -> float
            {
                if (value < min)
                    return min;
                if (value > max)
                    return max;
                return value;
            };

            const uint8_t r = static_cast<uint8_t>(clamp(color.r * 255.0f, 0.0f, 255.0f));
            const uint8_t g = static_cast<uint8_t>(clamp(color.g * 255.0f, 0.0f, 255.0f));
            const uint8_t b = static_cast<uint8_t>(clamp(color.b * 255.0f, 0.0f, 255.0f));
            const uint8_t a = static_cast<uint8_t>(clamp(color.a * 255.0f, 0.0f, 255.0f));
            const uint8_t pixel[4] = {r, g, b, a};

            for (size_t i = 0; i < size; i += channels)
            {
                std::memcpy(data + i, pixel, channels);
            }

            Generate(data, size, width, height, channels);

            delete[] data;
        }
        else
        {
            throw std::bad_alloc();
        }
    }

    void Texture2D::Generate(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels, const TextureSettings *settings)
    {
        if (data != nullptr)
        {
            this->width = width;
            this->height = height;

            if (!id)
                glGenTextures(1, &id);

            glBindTexture(GL_TEXTURE_2D, id);

            if (!settings)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                GLint glWrapS = (settings->wrapS == TextureWrapMode::Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
                GLint glWrapT = (settings->wrapT == TextureWrapMode::Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
                GLint glMin = GL_LINEAR;
                GLint glMag = (settings->magFilter == TextureFilterMode::Linear) ? GL_LINEAR : GL_NEAREST;

                switch (settings->minFilter)
                {
                case TextureFilterMode::Nearest:
                    glMin = GL_NEAREST;
                    break;
                case TextureFilterMode::Linear:
                    glMin = GL_LINEAR;
                    break;
                case TextureFilterMode::Trilinear:
                    glMin = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                case TextureFilterMode::BilinearMipmap:
                    glMin = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                default:
                    break;
                }

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMin);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMag);
            }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            switch (channels)
            {
            case 1:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                break;
            }
            case 2:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
                break;
            }
            case 3:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                break;
            }
            case 4:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            }
            default:
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &id);
                id = 0;
                std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(channels);
                throw std::invalid_argument(error);
            }
            }

            if (!settings)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                if (settings->minFilter == TextureFilterMode::Trilinear || settings->minFilter == TextureFilterMode::BilinearMipmap)
                    glGenerateMipmap(GL_TEXTURE_2D);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            throw std::invalid_argument("Failed to generate texture: data can not be null");
        }
    }

    void Texture2D::Destroy()
    {
        if (id)
            glDeleteTextures(1, &id);

        id = 0;
        width = 0;
        height = 0;
    }

    void Texture2D::Bind(uint32_t unit)
    {
        if (id)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, id);
        }
    }

    void Texture2D::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::SetMaxAnisotropyLevel(float level)
    {
        if (level > OpenGL::GetMaxAnisotropyLevel())
            level = OpenGL::GetMaxAnisotropyLevel();
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    uint32_t Texture2D::GetId() const
    {
        return id;
    }

    uint32_t Texture2D::GetWidth() const
    {
        return width;
    }

    uint32_t Texture2D::GetHeight() const
    {
        return height;
    }
}