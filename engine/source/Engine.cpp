#include "Engine.h"
#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render/Camera.h"

namespace eng
{
#if 0
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
#endif

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

		if (!CreateWindow(width, height, "The Sleipnir Engine"))
			return false;

		///////////////////////////////////////////////////////////////////////
		/// GLFW Callbacks for the input handlers
		///////////////////////////////////////////////////////////////////////
		glfwSetKeyCallback(m_window, KeyCallback);
		glfwSetFramebufferSizeCallback(m_window, OnEngineFrameBufferResize);
		glfwSetMouseButtonCallback(m_window, MousePressedCallback);
		glfwSetCursorPosCallback(m_window, MousePositionCallback);

		///////////////////////////////////////////////////////////////////////
		// ImGUI
		///////////////////////////////////////////////////////////////////////
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Platform/Renderer backends
		// ImGui installs its own GLFW callbacks and chains any previously-registered callbacks.
		// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		// install_callback=false, you will have to forward them manually
		ImGui_ImplGlfw_InitForOpenGL(m_window, false);
		ImGui_ImplOpenGL3_Init();

		// TODO added render queue init
		GetRenderQueue().Init();
		// TODO: define where to init. Ideally, there must be a distinction between a physical 3D camera during Gameplay, and the virtual "camera" that enables rendering on the Menu
		InitializeCamera();

		SetPlayMode(false);

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

			///////////////////////////////////////////////////////////////////////
			// ImGUI
			///////////////////////////////////////////////////////////////////////
			// Start the Dear ImGui frame
			int value = 0;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			/*ImGui::PushID("mag");
			ImGui::Text("Camera setting");
			ImGui::RadioButton("An option", &value, 0);
			ImGui::PopID();*/
			//ImGui::ShowDemoWindow(); // Show demo window! :)
			///////////////////////////////////////////////////////////////////////

			// Update game logic
			UpdateDeltaTime(deltaTime);

			m_activeCamera.Update(deltaTime);

			m_app->Update(deltaTime);

			m_graphicsApi.SetClearColor(1.f, 1.f, 1.f, 1.f);
			m_graphicsApi.ClearBuffers();

			m_renderQueue.Draw(m_graphicsApi);
			

			///////////////////////////////////////////////////////////////////////
			// ImGUI
			///////////////////////////////////////////////////////////////////////
			// Rendering
			// (Your code clears your framebuffer, renders your other stuff etc.)
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			// (Your code calls glfwSwapBuffers() etc.)
			///////////////////////////////////////////////////////////////////////

			// Handle rendering, pass the back buffer with the rendered image to the front buffer
			glfwSwapBuffers(m_window);

			// Update inputManager state
			if (GetInstance().m_isPlayMode)
				GetInputManager().SetLastMousePosition(GetInputManager().GetCurrentMousePosition());
		}
	};
	
	void Engine::Shutdown() // Previously Destroy
	{
		if (m_app)
		{
			///////////////////////////////////////////////////////////////////////
			// ImGUI
			///////////////////////////////////////////////////////////////////////
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			///////////////////////////////////////////////////////////////////////

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

	ActiveCamera&	Engine::GetMainCamera()
	{
		return m_activeCamera;
	}

	void Engine::SetPlayMode(const bool enable)
	{
			const Window gameWindow = GetInstance().g_window;
			const vec2 gameWindowCenter = {static_cast<int>(gameWindow.viewportWidth/2), static_cast<int>(gameWindow.viewportHeight/2)};

		if (enable)
		{
			GetInstance().GetInputManager().ResetMousePosition(gameWindowCenter);
			glfwSetCursorPos(m_window, gameWindowCenter.x, gameWindowCenter.y);
			GetInstance().GetInputManager().UpdateMouseMovementDelta();

			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(m_window, MousePositionCallback);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(m_window, nullptr);
		}

		m_isPlayMode = enable;
	}

	bool Engine::GetPlayMode() const
	{
		return GetInstance().m_isPlayMode;
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

		// Set the rendering context
		glfwMakeContextCurrent(m_window);

		// Initialize glew (OpenGL) and validate
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "ERROR: SETTING RENDERING CONTEXT FAILED \n";
			glfwTerminate();
			return false;
		}

		// Read GLFW framebuffer
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);
		g_window.pixelHeight = fbHeight;
		g_window.pixelWidth = fbWidth;

		return true;
	}

	void Engine::UpdateDeltaTime(float& timeToUpdate)
	{
		auto now = std::chrono::high_resolution_clock::now();
		timeToUpdate = std::chrono::duration<float>(now - m_lastTimePoint).count();
		m_lastTimePoint = now;
		m_deltaTime = timeToUpdate;
	}

	float Engine::GetDeltaTime() const
	{
		return m_deltaTime;
	}

	const World& Engine::GetWorld() const
	{
		return m_world;
	}

	const Window& Engine::GetWindow() const {
		return g_window;
	}

	void Engine::InitializeCamera()
	{
		std::cout << "{   x: " <<m_world.worldUp.x
					<< ", y: " <<m_world.worldUp.y
					<< ", z: " <<m_world.worldUp.z
					<< "}\n";

		PerspectiveParams	pp;
		pp.width        = g_window.pixelWidth;
		pp.height       = g_window.pixelHeight;
		pp.fov          = 45.0f;
		pp.nearPlane    = 0.1f;
		pp.farPlane     = 300.f;
		pp.aspectRatio  = static_cast<float>(pp.width) / static_cast<float>(pp.height);

		m_cameras[0] = new Camera(); // TODO make sure either the Engine destroys this
		m_cameras[0]->InitializeCamera(GetWorld().worldUp, pp);
		m_cameras[0]->SetCameraId(1);

		m_activeCamera.SetActiveCamera(*m_cameras[0] );

	}

	//////////////////////////////////////////////////////////////
	/// GLFW Callbacks
	//////////////////////////////////////////////////////////////

	void Engine::OnEngineFrameBufferResize(GLFWwindow* resizedWindow, int newPixelsWidth, int newPixelsHeight)
	{
		// It should call whatever holds the references to the list of FrameBuffers
		GetInstance().g_window.pixelWidth	= newPixelsWidth;
		GetInstance().g_window.pixelHeight	= newPixelsHeight;

		glfwGetWindowSize(resizedWindow, &GetInstance().g_window.viewportWidth, &GetInstance().g_window.viewportHeight);

		GetInstance().GetRenderQueue().ResizeRenderTargets(newPixelsWidth, newPixelsHeight);
	}

	void Engine::KeyCallback(GLFWwindow* window, int key, int scan, int action, int mod)
	{
		auto& inputManager = Engine::GetInstance().GetInputManager();

		if (action == GLFW_PRESS)
		{
			// Checks for play mode toggling first
			if (key == GLFW_KEY_TAB)
				GetInstance().SetPlayMode(!GetInstance().m_isPlayMode); // Toggles state
			else
				inputManager.SetKeyPressed(key, true);
		}
		else if (action == GLFW_RELEASE)
		{
			inputManager.SetKeyPressed(key, false);
		}
	}

	void Engine::MousePressedCallback(GLFWwindow* window, const int button, const int action, int mods)
	{

		//Forwarding Events to ImGUI
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

		if (!GetInstance().m_isPlayMode)
			return;

		auto& inputManager = GetInstance().GetInputManager();

		if (action == GLFW_PRESS)
			inputManager.SetMouseButtonPressed(button, true);
		else if (action == GLFW_RELEASE)
			inputManager.SetMouseButtonPressed(button, false);
	}

	void Engine::MousePositionCallback(GLFWwindow* window, const double xCoordinate, const double yCoordinate)
	{
		auto& inputManager = GetInstance().GetInputManager();
		inputManager.SetLastMousePosition(inputManager.GetCurrentMousePosition()); // Store the last state
		inputManager.SetCurrentMousePosition({static_cast<float>(xCoordinate), static_cast<float>(yCoordinate)}); // Update new state
		// Compute delta
		inputManager.UpdateMouseMovementDelta();

		auto& activeCamera = GetInstance().GetMainCamera();
		activeCamera.UpdateViewDirection(inputManager.GetMouseMovementDelta() * GetInstance().m_deltaTime  * 0.1f);
		//std::cout << "Mouse Position{ X:" << static_cast<float>(xCoordinate) << " Y:"<<static_cast<float>(yCoordinate) << " }\n";

		// Resetting the mouse position to avoid running out of room to rotate
		const Window gameWindow = GetInstance().g_window;
		const vec2 gameWindowCenter = {static_cast<int>(gameWindow.viewportWidth/2), static_cast<int>(gameWindow.viewportHeight/2)};

		GetInstance().GetInputManager().ResetMousePosition(gameWindowCenter);
		glfwSetCursorPos(window, gameWindowCenter.x, gameWindowCenter.y);
		GetInstance().GetInputManager().UpdateMouseMovementDelta();

	}


}
