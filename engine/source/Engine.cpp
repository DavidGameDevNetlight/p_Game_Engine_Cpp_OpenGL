#include "Engine.h"
#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render/Camera.h"

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

		if (!CreateWindow(width, height, "The Sleipnir Engine"))
			return false;

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
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		ImGui_ImplOpenGL3_Init();
		///////////////////////////////////////////////////////////////////////

		glfwSetFramebufferSizeCallback(m_window, OnEngineFrameBufferResize);

		// TODO added render queue init
		GetRenderQueue().Init();
		// TODO: define where to init. Ideally, there must be a distinction between a physical 3D camera during Gameplay, and the virtual "camera" that enables rendering on the Menu
		InitializeCamera();

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

			// Handle rendering
			glfwSwapBuffers(m_window);

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

	void Engine::OnEngineFrameBufferResize(GLFWwindow* resizedWindow, int newPixelsWidth, int newPixelsHeight)
	{
		// It should call whatever holds the references to the list of FrameBuffers
		GetInstance().g_window.pixelWidth	= newPixelsWidth;
		GetInstance().g_window.pixelHeight	= newPixelsHeight;

		GetInstance().GetRenderQueue().ResizeRenderTargets(newPixelsWidth, newPixelsHeight);
	}


}
