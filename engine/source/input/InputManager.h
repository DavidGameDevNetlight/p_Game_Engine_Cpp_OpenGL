#pragma once

#include <cstddef>
#include <glm/vec2.hpp>
/**
* This Class its owned and controlled by the Engine only
*/

using namespace glm;

namespace eng
{
	constexpr int KEYS_SIZE = 266;
	constexpr int MOUSE_KEYS_SIZE = 16;

	class InputManager
	{
	public:
		//////////////////////////////////////////
		/// Keyboard Actions
		//////////////////////////////////////////
		void SetKeyPressed( int key, bool pressed );
		bool IsKeyPressed( int key ) const;

		//////////////////////////////////////////
		/// Mouse Actions
		//////////////////////////////////////////
		bool IsMouseButtonPressed( int mouseKey ) const;
		void SetMouseButtonPressed( int mouseKey,  bool pressed );

		void SetLastMousePosition( const vec2& lastMousePosition );
		void SetCurrentMousePosition( const vec2& currentMousePosition );

		const vec2& GetLastMousePosition() const;
		const vec2& GetCurrentMousePosition() const;

		void UpdateMouseMovementDelta();
		const vec2& GetMouseMovementDelta() const;

		bool IsRightMouseButtonPressed() const;
		bool IsLeftMouseButtonPressed() const;

		void ResetMousePosition(const vec2& resetPosition);
		/*
		 * NOTE regarding const primitive arguments:
		 * The compiler allows for const protection in the .cpp definition, while the .h declaration remains clean
		 *
		 * NOTE about returning const built-in primitives:
		 * The compiler creates a copy anyway, and the caller can modify that copy, without altering the member's value
		 */

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


		//////////////////////////////////////////
		/// Memory for GLFW keys
		//////////////////////////////////////////
		const	size_t	m_keysSize			= KEYS_SIZE;
		const	size_t	m_mouseKeysSize		= MOUSE_KEYS_SIZE;

		bool	m_keys[KEYS_SIZE]				= { false };
		bool    m_mouseKeys[MOUSE_KEYS_SIZE]	= { false };

		vec2 m_lastMousePosition		= {0, 0};
		vec2 m_currentMousePosition		= {0, 0};
		vec2 m_mouseMovementDelta		= {0, 0};
		
	};
}