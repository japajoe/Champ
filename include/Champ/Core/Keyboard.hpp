#pragma once

#include "KeyCode.hpp"
#include <cstdint>
#include <unordered_map>

namespace Champ
{
    struct KeyState
    {
        int32_t down;
        int32_t up;
        int32_t pressed;
        int32_t state;
        double lastRepeatTime;
        bool repeat;

        KeyState()
        {
            down = 0;
            up = 0;
            pressed = 0;
            state = 0;
            lastRepeatTime = 0.0;
            repeat = false;
        }
    };

    class Keyboard
    {
    public:
        Keyboard();
        void NewFrame();
        void SetState(KeyCode keycode, int32_t state);
        void AddInputCharacter(uint32_t codepoint);
        bool GetKey(KeyCode keycode);
        bool GetKeyDown(KeyCode keycode);
        bool GetKeyUp(KeyCode keycode);
        bool IsAnyKeyPressed();
        bool GetAnyKeyDown(KeyCode &keycode);

    private:
        std::unordered_map<KeyCode, KeyState> states;
        double repeatDelay;
        double repeatInterval;
    };
}