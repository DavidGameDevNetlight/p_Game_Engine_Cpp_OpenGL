#include "input/InputManager.h"

#include <iostream>

#include "GLFW/glfw3.h"

namespace eng
{
	//////////////////////////////////////////
	/// Keyboard Actions
	//////////////////////////////////////////

	void InputManager::SetKeyPressed(const int key, const bool pressed)
	{
		if (key < 0 || key >= m_keysSize)
			return;

		m_keys[key] = pressed;
	}

	bool InputManager::IsKeyPressed(const int key) const
	{
		if (key < 0 || key >= m_keysSize)
			return false;

		return m_keys[key];
	}

	//////////////////////////////////////////
	/// Mouse Button Actions
	//////////////////////////////////////////

	void InputManager::SetMouseButtonPressed(const int mouseKey, const bool pressed)
	{
		std::cout << "Mouse Button Event was triggered: " << pressed << std::endl;

		if (mouseKey < 0 || mouseKey >= m_mouseKeysSize)
			return;

		m_mouseKeys[mouseKey] = pressed;
	}

	bool InputManager::IsMouseButtonPressed(const int mouseKey) const
	{
		if (mouseKey < 0 || mouseKey >= m_mouseKeysSize)
			return false;

		return m_mouseKeys[mouseKey];
	}

	bool InputManager::IsRightMouseButtonPressed()	const { return IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT); }
	bool InputManager::IsLeftMouseButtonPressed()	const { return IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);  }

	void InputManager::ResetMousePosition(const vec2& resetPosition)
	{
		m_currentMousePosition = vec2(resetPosition);
		m_lastMousePosition = m_currentMousePosition;
	}
	//////////////////////////////////////////
	/// Mouse Movement Actions
	//////////////////////////////////////////

	void InputManager::SetLastMousePosition( const vec2& lastMousePosition ) { m_lastMousePosition = vec2(lastMousePosition); }
	void InputManager::SetCurrentMousePosition( const vec2& currentMousePosition ) { m_currentMousePosition = vec2(currentMousePosition); }

	const vec2& InputManager::GetLastMousePosition() const { return m_lastMousePosition; }
	const vec2& InputManager::GetCurrentMousePosition() const { return m_currentMousePosition; }
	const vec2& InputManager::GetMouseMovementDelta() const { return m_mouseMovementDelta; }

	void InputManager::UpdateMouseMovementDelta() { m_mouseMovementDelta = vec2(m_currentMousePosition - m_lastMousePosition); }

}
