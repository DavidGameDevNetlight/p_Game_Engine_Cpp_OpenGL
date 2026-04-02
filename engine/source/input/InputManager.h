#pragma once

#include <cstddef>

/**
* This Class its owned and controlled by the Engine only
*/

namespace eng
{
	constexpr int KEYS_SIZE = 266;

	class InputManager
	{
	public:
				void SetKeyPressed(int key, bool pressed);
		const	bool IsKeyPressed(int key);

	private:

		//////////////////////////////////////////
		// Private Constructors and Destructors
		//////////////////////////////////////////
		InputManager() = default;
		InputManager(const InputManager&) = delete;		// Copy constructor deleted
		InputManager(InputManager&&) = delete;			// Move constructor deleted

		InputManager& operator = (const InputManager&) = delete;	// Copy operator deleted
		InputManager& operator = (InputManager&&) = delete;			// Move operator deleted

		//////////////////////////////////////////
		// Friend declaration
		//////////////////////////////////////////
		friend class Engine; // only Engine can access and destroy InputManager
		// It also allows the Engine to call the default constructor when declaring InputManager as a member variable

				bool	m_keys[KEYS_SIZE]	= { false };
		const	size_t	m_keysSize			= KEYS_SIZE;
		
	};
}