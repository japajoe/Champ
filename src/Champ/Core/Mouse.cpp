#include "Mouse.hpp"
#include <GLFW/glfw3.h>

namespace Champ
{
    Mouse::Mouse(GLFWwindow *window)
    {
        this->window = window;
        repeatDelay = 0.5;      // Delay before repeat starts
        repeatInterval = 0.025; // Interval for repeat
        positionX = 0.0f;
        positionY = 0.0f;
        windowPositionX = 0.0f;
        windowPositionY = 0.0f;
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollY = 0.0f;
        scrollX = 0.0f;
        cursorVisible = true;

        states[ButtonCode::Left] = ButtonState();
        states[ButtonCode::Right] = ButtonState();
        states[ButtonCode::Middle] = ButtonState();
    }

    void Mouse::NewFrame()
    {
        for (auto &k : states)
        {
            ButtonCode button = k.first;
            int state = k.second.state;

            if (state > 0)
            {
                if (states[button].down == 0)
                {
                    states[button].down = 1;
                    states[button].pressed = 0;
                    states[button].lastRepeatTime = glfwGetTime();
                    states[button].repeat = false;
                }
                else
                {
                    states[button].down = 1;
                    states[button].pressed = 1;

                    double currentTime = glfwGetTime();
                    double elapsed = currentTime - states[button].lastRepeatTime;

                    if (!states[button].repeat)
                    {
                        if (elapsed >= repeatDelay)
                        {
                            states[button].repeat = true;
                            states[button].lastRepeatTime = currentTime;
                        }
                    }
                    else
                    {
                        if (elapsed >= repeatInterval)
                        {
                            states[button].lastRepeatTime = currentTime;
                        }
                    }
                }

                states[button].up = 0;
            }
            else
            {
                if (states[button].down == 1 || states[button].pressed == 1)
                {
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 1;
                }
                else
                {
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 0;
                }
            }
        }
    }

    void Mouse::EndFrame()
    {
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollX = 0.0f;
        scrollY = 0.0f;
    }

    void Mouse::SetPosition(float x, float y)
    {
        float prevX = positionX;
        float prevY = positionY;

        positionX = x;
        positionY = y;

        deltaX = x - prevX;
        deltaY = y - prevY;
    }

    void Mouse::SetWindowPosition(float x, float y)
    {
        windowPositionX = x;
        windowPositionY = y;
    }

    void Mouse::SetScrollDirection(float x, float y)
    {
        scrollX = x;
        scrollY = y;
    }

    void Mouse::SetCursor(bool visible)
    {
        glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        cursorVisible = visible;
    }

    void Mouse::SetState(ButtonCode buttoncode, int32_t state)
    {
        states[buttoncode].state = state;
    }

    bool Mouse::GetButton(ButtonCode buttoncode)
    {
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 1;
    }

    bool Mouse::GetButtonDown(ButtonCode buttoncode)
    {
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 0;
    }

    bool Mouse::GetButtonUp(ButtonCode buttoncode)
    {
        ButtonState state = states[buttoncode];
        return state.up > 0;
    }

    bool Mouse::GetAnyButtonDown(ButtonCode &buttoncode)
    {
        for (auto &item : states)
        {
            if (GetButtonDown(item.first))
            {
                buttoncode = item.first;
                return true;
            }
        }

        return false;
    }

    float Mouse::GetX() const
    {
        return positionX;
    }

    float Mouse::GetY() const
    {
        return positionY;
    }

    float Mouse::GetAbsoluteX() const
    {
        return windowPositionX + positionX;
    }

    float Mouse::GetAbsoluteY() const
    {
        return windowPositionY + positionY;
    }

    float Mouse::GetDeltaX() const
    {
        return deltaX;
    }

    float Mouse::GetDeltaY() const
    {
        return deltaY;
    }

    float Mouse::GetScrollX() const
    {
        return scrollX;
    }

    float Mouse::GetScrollY() const
    {
        return scrollY;
    }

    bool Mouse::IsCursorVisible() const
    {
        return cursorVisible;
    }
}