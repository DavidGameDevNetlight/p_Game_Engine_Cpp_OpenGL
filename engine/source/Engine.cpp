#include "Engine.h"
#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace eng
{
	void keyCallback(GLFWwindow* window, int key, int scan, int action, int mod)
	{
		auto& inputManager = Engine::GetInstance().GetInputManager();

		if (action == GLFW_PRESS)
		{
			inputManager.SetKeyPressed(key, true);
		}
		else if (action == GLFW_RELEASE)
		{
			inputManager.SetKeyPressed(key, false);
		}
	}

	Engine& Engine::GetInstance()
	{
		static Engine instance;
		return instance;
	}

	Engine::~Engine()
	{
		delete m_app;
		m_app = nullptr;
	}

	bool Engine::Init(unsigned int width, unsigned int height)
	{
		if (!m_app)
			return false;

		if (!CreateWindow(width, height, "Profetics Engine"))
			return false;

		return m_app->Init();
	};

	void Engine::Run()
	{
		if (!m_app)
			return;

		m_lastTimePoint = std::chrono::high_resolution_clock::now();
		float deltaTime = 0.f;

		///////////////////////////////////////////////////////////////////////
		// THE GAME LOOP
		///////////////////////////////////////////////////////////////////////
		while (!glfwWindowShouldClose(m_window) && !m_app->NeedsToBeClosed())
		{
			// Handle user inputs
			glfwPollEvents();
			
			// Update game logic
			UpdateDeltaTime(deltaTime);
			m_app->Update(deltaTime);

			m_graphicsApi.SetClearColor(0.f, 0.f, 0.f, 1.f);
			m_graphicsApi.ClearBuffers();

			m_renderQueue.Draw(m_graphicsApi);
			
			// Handle rendering
			glfwSwapBuffers(m_window);

		}
	};
	
	void Engine::Shutdown() // Previously Destroy
	{
		if (m_app)
		{
			m_app->Destroy(); // logical clean up only
			glfwTerminate();
			m_window = nullptr;
		}
	
	};

	void Engine::SetApplication(Application *app)
	{
		delete m_app; 	// Delete previous
		m_app = app;	// Takes ownership
	};

	Application* Engine::GetApplication() const
	{
		return m_app;
	}

	InputManager& Engine::GetInputManager()
	{
		return m_inputManager;
	}

	GraphicsAPI& Engine::GetGraphicsAPI()
	{
		return m_graphicsApi;
	}

	RenderQueue& Engine::GetRenderQueue()
	{
		return m_renderQueue;
	}

	///////////////////////////////////////////////////////////////////
	// Privates
	///////////////////////////////////////////////////////////////////

	bool Engine::CreateWindow(const unsigned int width, const unsigned int height, const char* title)
	{
		if (width == 0 || height == 0)
		{
			std::cout << "ERROR: The window size must be grater than 0 \n";
			return false;
		}

		// Initialize GLWF
		if (!glfwInit())
			return false;
		
		// Set OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glewExperimental = GL_TRUE;
#endif
		// Create a window and validate
		m_window = glfwCreateWindow( width, height, title, nullptr, nullptr);
		if (m_window == nullptr)
		{
			std::cerr << "ERROR WINDOW CREATION FAILED \n";
			glfwTerminate();
			return false;
		}

		// Pass the callback function for the input handlers
		glfwSetKeyCallback(m_window, keyCallback);

		// Set the rendering context
		glfwMakeContextCurrent(m_window);

		// Initialize glew (OpenGL) and validate
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "ERROR: SETTING RENDERING CONTEXT FAILED \n";
			glfwTerminate();
			return false;
		}

		return true;
	}

	void Engine::UpdateDeltaTime(float& timeToUpdate)
	{
		auto now = std::chrono::high_resolution_clock::now();
		timeToUpdate = std::chrono::duration<float>(now - m_lastTimePoint).count();
		m_lastTimePoint = now;
	}

}
