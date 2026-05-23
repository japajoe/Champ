#include "Color.hpp"
#include <random>

namespace Champ
{
    Color::Color()
    {
        this->r = 1.0;
        this->g = 1.0;
        this->b = 1.0;
        this->a = 1.0;
    }

    Color::Color(float r, float g, float b, float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color::Color(float r, float g, float b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 1.0f;
    }

    Color::Color(int r, int g, int b, int a)
    {
        this->r = Normalize(r);
        this->g = Normalize(g);
        this->b = Normalize(b);
        this->a = Normalize(a);
    }

    float Color::Normalize(int value)
    {
        if (value <= 0)
            return 0;
        if (value <= 1.0f)
            return (float)value;
        return (float)value / 255.0f;
    }

    Color Color::FromHex(uint32_t hex)
    {
        float r = ((hex >> 24) & 0xFF) / 255.0f;
        float g = ((hex >> 16) & 0xFF) / 255.0f;
        float b = ((hex >> 8) & 0xFF) / 255.0f;
        float a = ((hex >> 0) & 0xFF) / 255.0f;

        return Color(r, g, b, a);
    }

    Color Color::Lerp(const Color &a, const Color &b, float t)
    {
        Color c;

        auto lerp = [](float a, float b, float t) -> float
        {
            return a + (b - a) * t;
        };

        c.r = lerp(a.r, b.r, t);
        c.g = lerp(a.g, b.g, t);
        c.b = lerp(a.b, b.b, t);
        c.a = lerp(a.a, b.a, t);
        return c;
    }

    Color Color::ToGrayScale(const Color &color)
    {
        float r = Normalize(color.r);
        float g = Normalize(color.g);
        float b = Normalize(color.b);
        float a = Normalize(color.a);

        float v = (r + g + b);
        if (v > 0.0f)
            v /= 3.0f;
        Color c;
        c.r = v;
        c.g = v;
        c.b = v;
        c.a = a;
        return c;
    }

    Color Color::operator+(const Color &other) const
    {
        return {r + other.r, g + other.g, b + other.b, a + other.a};
    }

    Color Color::operator-(const Color &other) const
    {
        return {r - other.r, g - other.g, b - other.b, a - other.a};
    }

    Color Color::operator*(float scalar) const
    {
        return {r * scalar, g * scalar, b * scalar, a * scalar};
    }

    Color Color::operator/(float scalar) const
    {
        if (scalar == 0.0f)
        {
            return {0.0f, 0.0f, 0.0f, 0.0f};
        }
        return {r / scalar, g / scalar, b / scalar, a / scalar};
    }

    Color &Color::operator+=(const Color &other)
    {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
    }

    Color &Color::operator-=(const Color &other)
    {
        r -= other.r;
        g -= other.g;
        b -= other.b;
        a -= other.a;
        return *this;
    }

    Color &Color::operator*=(float scalar)
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }

    Color &Color::operator/=(float scalar)
    {
        if (scalar != 0.0f)
        {
            r /= scalar;
            g /= scalar;
            b /= scalar;
            a /= scalar;
        }
        return *this;
    }

    Color Color::LightGray()
    {
        return Color(200, 200, 200, 255);
    }

    Color Color::Gray()
    {
        return Color(130, 130, 130, 255);
    }

    Color Color::DarkGray()
    {
        return Color(80, 80, 80, 255);
    }

    Color Color::Yellow()
    {
        return Color(253, 249, 0, 255);
    }

    Color Color::Gold()
    {
        return Color(255, 203, 0, 255);
    }

    Color Color::Orange()
    {
        return Color(255, 161, 0, 255);
    }

    Color Color::Pink()
    {
        return Color(255, 109, 194, 255);
    }

    Color Color::Red()
    {
        return Color(255, 0, 0, 255);
    }

    Color Color::Maroon()
    {
        return Color(190, 33, 55, 255);
    }

    Color Color::Green()
    {
        return Color(0, 255, 0, 255);
    }

    Color Color::Lime()
    {
        return Color(0, 158, 47, 255);
    }

    Color Color::LightGreen()
    {
        return Color(13, 224, 77, 255);
    }

    Color Color::DarkGreen()
    {
        return Color(0, 117, 44, 255);
    }

    Color Color::SkyBlue()
    {
        return Color(102, 191, 255, 255);
    }

    Color Color::Blue()
    {
        return Color(0, 0, 255, 255);
    }

    Color Color::DarkBlue()
    {
        return Color(0, 82, 172, 255);
    }

    Color Color::Purple()
    {
        return Color(200, 122, 255, 255);
    }

    Color Color::Violet()
    {
        return Color(135, 60, 190, 255);
    }

    Color Color::DarkPurple()
    {
        return Color(112, 31, 126, 255);
    }

    Color Color::Beige()
    {
        return Color(211, 176, 131, 255);
    }

    Color Color::Brown()
    {
        return Color(127, 106, 79, 255);
    }

    Color Color::DarkBrown()
    {
        return Color(76, 63, 47, 255);
    }

    Color Color::White()
    {
        return Color(255, 255, 255, 255);
    }

    Color Color::Black()
    {
        return Color(0, 0, 0, 255);
    }

    Color Color::Blank()
    {
        return Color(0, 0, 0, 0);
    }

    Color Color::Cyan()
    {
        return Color(0, 255, 255, 255);
    }

    Color Color::Magenta()
    {
        return Color(255, 0, 255, 255);
    }

    Color Color::RayWhite()
    {
        return Color(245, 245, 245, 255);
    }

    static std::random_device gRandomDevice;
    static std::mt19937 gGenerator(gRandomDevice());
    static std::uniform_real_distribution<float> gDistribution(0.0f, 1.0f);

    void Color::SetRandomSeed(uint32_t seed)
    {
        gGenerator.seed(seed);
    }

    Color Color::GetRandom()
    {
        Color color;
        color.r = gDistribution(gGenerator);
        color.g = gDistribution(gGenerator);
        color.b = gDistribution(gGenerator);
        color.a = 1.0f;
        return color;
    }

    std::string Color::ToString(const Color &color)
    {
        std::string s = std::to_string(color.r) + ", " +
                        std::to_string(color.g) + ", " +
                        std::to_string(color.b) + ", " +
                        std::to_string(color.a);
        return s;
    }
}