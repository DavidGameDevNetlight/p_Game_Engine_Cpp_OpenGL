#pragma once
#include <chrono>
#include <iostream>
#include "input/InputManager.h"
#include "graphics/GraphicsAPI.h"
#include "render/RenderQueue.h"

/////////////////////////////////////////////////////////
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// Engine class
/**
 * Owns Application
 * In charge of deleting Application, ALWAYS
 * No other class should delete Application
 * Only one owner
 */
/////////////////////////////////////////////////////////
constexpr unsigned int DEFALT_WINDOW_WIDTH = 1280;
constexpr unsigned int DEFALT_WINDOW_HEIGHT = 720;
struct GLFWwindow; // Forward declaration on a global scope, because glfw is a C library without namespaces.

namespace eng
{
	/// Forward declaration:
	// This tells the compiler: "A class called Application exists I'll give you its full definition later."
	class Application;

	/// <summary>
	/// Singleton class
	/// </summary>
	class Engine
	{
	private:
		// Make all constructors private so the instance can only be created form inside the class.
		Engine() = default;

		// The following deletions tell the compiler that this functions exits but should not be called, throwing an error
		// This enforces the behaviour and use of the singleton
		Engine(const Engine&) = delete;		// Copy constructor deleted
		Engine(Engine&&) = delete;			// Move constructor deleted

		Engine& operator = (const Engine&) = delete;	// Copy operator deleted
		Engine& operator = (Engine&&) = delete;			// Move operator deleted

		~Engine();

	public:
		/// @brief Meyer's Singleton, creates an instance in static memory the frist time is it created
		/// @return 
		static Engine& GetInstance();

		/// <summary>
		/// Executes Init in the game instance.
		/// Separates "object exists" from "object ready for use"
		/// It uses a default window size if parameters are not passed in
		/// </summary>
		bool Init(unsigned int width = DEFALT_WINDOW_WIDTH, unsigned int height = DEFALT_WINDOW_HEIGHT);
		
		/// <summary>
		///	Contains the Main Game Loop: Input, Game State/Logic, Sound and Rendering
		/// </summary>
		void Run();

		/// <summary>
		/// Note: the Game instance and heap-allocated memory is freed inside ~Engine
		/// </summary>
		void Shutdown(); // previously Destroy, logical shutdown only

		/// <summary>
		/// Ownership transferred happens here. Gets the pointer to the Game instance created by main.cpp,
		/// and Engine in charge of deleting Game.
		/// </summary>
		void SetApplication(Application* app);	// ownership transfer

		float GetDeltaTime() const;
		//////////////////////////////////////////////////////////////
		// Getters to main classes
		//////////////////////////////////////////////////////////////

		Application*		GetApplication() const;	// non-owning access
		InputManager&		GetInputManager(); // Const Engine, read-only access
		GraphicsAPI&		GetGraphicsAPI();
		RenderQueue&		GetRenderQueue();
	
	private:
		std::chrono::steady_clock::time_point m_lastTimePoint;
		
		Application*	m_app			= nullptr; // original member type: std::unique_ptr<Application>
		GLFWwindow*		m_window		= nullptr;
		
		// The Engine will call InputManger() default constructor because its a friend class
		InputManager	m_inputManager;	

		GraphicsAPI		m_graphicsApi;

		RenderQueue		m_renderQueue;

		/// <summary>
		/// Uses GLWF to create the window and set the OpenGL context
		/// </summary>
		/// <returns>True, if window succeed</returns>
		bool CreateWindow( const unsigned int width, const unsigned int height, const char* title);

		void UpdateDeltaTime(float& timeToUpdate);

		///////////////////////////////////////////////////////
		// Time Features
		///////////////////////////////////////////////////////
		float m_deltaTime		= 0.0f;
		float m_timeInSeconds	= 0.0f;
	};

}