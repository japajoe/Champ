#pragma once

#include "ButtonCode.hpp"
#include <cstdint>
#include <unordered_map>

struct GLFWwindow;

namespace Champ
{
    struct ButtonState
    {
        int32_t down;
        int32_t up;
        int32_t pressed;
        int32_t state;
        double lastRepeatTime;
        bool repeat;

        ButtonState()
        {
            down = 0;
            up = 0;
            pressed = 0;
            state = 0;
            lastRepeatTime = 0.0;
            repeat = false;
        }
    };

    class Mouse
    {
    public:
        Mouse(GLFWwindow *window);
        void NewFrame();
        void EndFrame();
        void SetPosition(float x, float y);
        void SetWindowPosition(float x, float y);
        void SetScrollDirection(float x, float y);
        void SetCursor(bool visible);
        void SetState(ButtonCode buttoncode, int32_t state);
        bool GetButton(ButtonCode buttoncode);
        bool GetButtonDown(ButtonCode buttoncode);
        bool GetButtonUp(ButtonCode buttoncode);
        bool GetAnyButtonDown(ButtonCode &keycode);
        float GetX() const;
        float GetY() const;
        float GetAbsoluteX() const;
        float GetAbsoluteY() const;
        float GetDeltaX() const;
        float GetDeltaY() const;
        float GetScrollX() const;
        float GetScrollY() const;
        bool IsCursorVisible() const;

    private:
        std::unordered_map<ButtonCode, ButtonState> states;
        float positionX;
        float positionY;
        float windowPositionX;
        float windowPositionY;
        float deltaX;
        float deltaY;
        float scrollY;
        float scrollX;
        double repeatDelay;
        double repeatInterval;
        bool cursorVisible;
        GLFWwindow *window;
    };
}