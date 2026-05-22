#pragma once

#include <string>
#include <cstdint>

namespace Champ
{
    struct Color
    {
        float r;
        float g;
        float b;
        float a;
        Color();
        Color(float r, float g, float b, float a);
        Color(float r, float g, float b);
        Color(int r, int g, int b, int a);
        Color operator+(const Color &other) const;
        Color operator-(const Color &other) const;
        Color operator*(float scalar) const;
        Color operator/(float scalar) const;
        Color &operator+=(const Color &other);
        Color &operator-=(const Color &other);
        Color &operator*=(float scalar);
        Color &operator/=(float scalar);
        static float Normalize(int value);
        static Color FromHex(uint32_t hex);
        static Color Lerp(const Color &a, const Color &b, float t);
        static Color ToGrayScale(const Color &color);
        static Color LightGray();
        static Color Gray();
        static Color DarkGray();
        static Color Yellow();
        static Color Gold();
        static Color Orange();
        static Color Pink();
        static Color Red();
        static Color Maroon();
        static Color Green();
        static Color Lime();
        static Color LightGreen();
        static Color DarkGreen();
        static Color SkyBlue();
        static Color Blue();
        static Color DarkBlue();
        static Color Purple();
        static Color Violet();
        static Color DarkPurple();
        static Color Beige();
        static Color Brown();
        static Color DarkBrown();
        static Color White();
        static Color Black();
        static Color Blank();
        static Color Cyan();
        static Color Magenta();
        static Color RayWhite();
        static std::string ToString(const Color &color);
    };
}