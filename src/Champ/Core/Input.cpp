#include "Input.hpp"
#include <GLFW/glfw3.h>
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include <memory>

namespace Champ
{
	AxisKeys::AxisKeys()
	{
	}

	AxisKeys::AxisKeys(KeyCode positive, KeyCode negative)
	{
		this->positive = positive;
		this->negative = negative;
	}

	AxisInfo::AxisInfo()
	{
	}

	AxisInfo::AxisInfo(const std::string &name)
	{
		this->name = name;
	}

	void AxisInfo::AddKeys(KeyCode positive, KeyCode negative)
	{
		this->keys.push_back(AxisKeys(positive, negative));
	}

	static GLFWwindow *gWindow = nullptr;
	static std::unique_ptr<Keyboard> gKeyboard;
	static std::unique_ptr<Mouse> gMouse;
	static std::unordered_map<std::string, AxisInfo> gKeyToAxisMap;

	void Input::Initialize(GLFWwindow *window)
	{
		gWindow = window;

		gKeyboard = std::make_unique<Keyboard>();
		gMouse = std::make_unique<Mouse>(window);

		AxisInfo axisHorizontal("Horizontal");
		AxisInfo axisVertical("Vertical");
		AxisInfo axisPanning("Panning");

		axisHorizontal.AddKeys(KeyCode::D, KeyCode::A);
		axisVertical.AddKeys(KeyCode::W, KeyCode::S);
		axisPanning.AddKeys(KeyCode::R, KeyCode::F);

		RegisterAxis(axisHorizontal);
		RegisterAxis(axisVertical);
		RegisterAxis(axisPanning);
	}

	void Input::NewFrame()
	{
		// Cursorpos callback is not called when mouse is outside the bounds of the window
		double cursorPosX, cursorPosY;
		glfwGetCursorPos(gWindow, &cursorPosX, &cursorPosY);
		SetMousePosition(cursorPosX, cursorPosY);

		gKeyboard->NewFrame();
		gMouse->NewFrame();
	}

	void Input::EndFrame()
	{
		gMouse->EndFrame();
	}

	void Input::SetMousePosition(double x, double y)
	{
		gMouse->SetPosition(x, y);
	}

	void Input::SetWindowPosition(double x, double y)
	{
		gMouse->SetWindowPosition(x, y);
	}

	void Input::SetKeyState(KeyCode keycode, int state)
	{
		gKeyboard->SetState(keycode, state);
	}

	void Input::AddInputCharacter(uint32_t codepoint)
	{
		gKeyboard->AddInputCharacter(codepoint);
	}

	void Input::SetButtonState(ButtonCode buttoncode, int state)
	{
		gMouse->SetState(buttoncode, state);
	}

	void Input::SetScrollDirection(double x, double y)
	{
		gMouse->SetScrollDirection(x, y);
	}

	void Input::RegisterAxis(const AxisInfo &axisInfo)
	{
		if (gKeyToAxisMap.count(axisInfo.name))
			return;
		gKeyToAxisMap[axisInfo.name] = axisInfo;
	}

	float Input::GetAxis(const std::string &axis)
	{
		if (gKeyToAxisMap.count(axis))
		{
			for (size_t i = 0; i < gKeyToAxisMap[axis].keys.size(); i++)
			{
				if (GetKey(gKeyToAxisMap[axis].keys[i].positive))
					return 1.0f;
				else if (GetKey(gKeyToAxisMap[axis].keys[i].negative))
					return -1.0f;
			}
		}

		return 0.0f;
	}

	bool Input::GetKey(KeyCode keycode)
	{
		return gKeyboard->GetKey(keycode);
	}

	bool Input::GetKeyDown(KeyCode keycode)
	{
		return gKeyboard->GetKeyDown(keycode);
	}

	bool Input::GetKeyUp(KeyCode keycode)
	{
		return gKeyboard->GetKeyUp(keycode);
	}

	bool Input::GetAnyKeyDown(KeyCode &keycode)
	{
		return gKeyboard->GetAnyKeyDown(keycode);
	}

	bool Input::GetButton(ButtonCode buttoncode)
	{
		return gMouse->GetButton(buttoncode);
	}

	bool Input::GetButtonDown(ButtonCode buttoncode)
	{
		return gMouse->GetButtonDown(buttoncode);
	}

	bool Input::GetButtonUp(ButtonCode buttoncode)
	{
		return gMouse->GetButtonUp(buttoncode);
	}

	bool Input::GetAnyButtonDown(ButtonCode &buttoncode)
	{
		return gMouse->GetAnyButtonDown(buttoncode);
	}

	Vector2 Input::GetScrollDirection()
	{
		return Vector2(gMouse->GetScrollX(), gMouse->GetScrollY());
	}

	Vector2 Input::GetMousePosition()
	{
		return Vector2(gMouse->GetX(), gMouse->GetY());
	}

	Vector2 Input::GetMouseDelta()
	{
		return Vector2(gMouse->GetDeltaX(), gMouse->GetDeltaY());
	}

	void Input::SetMouseCursor(bool visible)
	{
		gMouse->SetCursor(visible);
	}

	bool Input::IsCursorVisible()
	{
		return gMouse->IsCursorVisible();
	}
}