#include "render/RenderQueue.h"

#include "glm/fwd.hpp"
#include "graphics/GraphicsAPI.h"
#include "render/Material.h"
#include "render/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include <fstream>
#include "GLFW/glfw3.h"
#include "graphics/ShaderProgram.h"
#include "renderHelper/Model.h"
#include "Camera.h"
using namespace glm;

namespace eng
{
	//TODO: all of these are temporal debug variables that will be moved

	FrameBuffer				offScreenRenderTarget;
	struct FrameBuffer;
	std::vector<FrameBuffer> fboList;
	///////////////////////////////////////////////////////////////////////////////
	// Framebuffers
	///////////////////////////////////////////////////////////////////////////////



	renderhelper::Model*	sphereModel						= nullptr;
	// To shade meshes with the Bling-Phong Illumination model
	ShaderProgram*			debugShaderProgram				= nullptr;
	// To shade meshes with solid colors
	ShaderProgram*			debugColorShaderProgram			= nullptr;
	// To shade the near plane with an infinite background
	ShaderProgram*			debugScreenSpaceShaderProgram	= nullptr;
	// To shade sample the scene texture (color texture) and display the fina image to the default framebuffer
	ShaderProgram*			debugCompositeShaderProgram		= nullptr;

	Mesh*					debugScreenSpaceMesh			= nullptr;

	float			 		environment_multiplier			= 1.0f;
	GLuint			 		environmentMap;
	GLuint			 		irradianceMap;
	GLuint			 		reflectionMap;
				 		std::string envmap_base_name	= "001";

	vec3	gLightDir = normalize(vec3(0.0f, -1.0f, -1.0f));
	vec3	spherePos = vec3(0.0f);
	mat4	sphereModelMatrix(	1.0f, 0.0f, 0.0f, 0.0f, // x
							0.0f, 1.0f, 0.0f, 0.0f, // y
							0.0f, 0.0f, 1.0f, 0.0f, // z
							0.0f, 0.0f, 0.0f, 1.0f); // translation

	void FrameBuffer::InitializeFrameBuffer(const int viewPortWidth, const int viewPortHeight) {
		width = viewPortWidth;
		height = viewPortHeight;
		//////////////////////////////////////////
		/// Generate color and depth texture
		//////////////////////////////////////////
		GenerateColorBuffer(width, height);
		GenerateDepthBuffer(width, height);
		//////////////////////////////////////////
		/// Generate frame buffer
		//////////////////////////////////////////
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		//////////////////////////////////////////
		/// Validate frame buffer
		//////////////////////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
			std::cerr<<"Framebuffer not complete \n";
		//return (status == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::GenerateColorBuffer(const int viewPortWidth, const int viewPortHeight)
	{
		glGenTextures(1, &colorTexture);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		//! NOTE: glTextureParameteri this is the Direct State Acces variant of glTexParameteri
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	void FrameBuffer::GenerateDepthBuffer(const int viewPortWidth, const int viewPortHeight)
	{
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	void FrameBuffer::resize(int w, int h)
	{
		width = w;
		height = h;
		// Allocate a texture
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// generate a depth texture
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
					 nullptr);
	}

	void FrameBuffer::SetAsRenderTarget() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glViewport(0,0, width, height); // The size of the window to render
		glClearColor(1.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	RenderQueue::~RenderQueue()
	{
		// For the record, this is a commun mistake, a raw array is NOT heap allocated unless it
		// was created using new. Even if what it stores are pointers to heap allocated memory
		// delete[] m_renderCommands;
		delete debugScreenSpaceMesh;
		delete debugShaderProgram;
		delete debugScreenSpaceShaderProgram;
		delete debugColorShaderProgram;
		delete debugCompositeShaderProgram;
	}

	
	void RenderQueue::Submit(const RenderCommand& command)
	{
		// This local assignation give the static analysis engine a safety way to validate
		// against potential buffer overrun. All arrays using this index are safe.
		const size_t index = m_commandsCount;

		// Invariance
		if (index >= RENDER_COMMANDS_SIZE)
		{
			std::cerr << "ERROR: RenderQueue is full! Command dropped.\n";
			return;
		}

		// Shallow copies here, only copying the pointers to the same mesh and material objects
		m_renderCommands[index].mesh		= command.mesh;
		m_renderCommands[index].material	= command.material;
		m_usedCommands[index] = true;

		m_commandsCount++;
	}

	void RenderGUI()
	{

	}


	void loadShaders()
	{
		//////////////////////////////////////////////////////////////
		// Shader sources for debugging
		//////////////////////////////////////////////////////////////

		//std::ifstream   vertexShaderFile("./assets/debug_lambert_diffuse.vert");
		//std::ifstream   fragmentShaderFile("./assets/debug_lambert_diffuse.frag");
		// This long none-build path is a workaround to enable shader reload at runtime
		std::ifstream   vertexShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_lambert_diffuse.vert");
		std::ifstream   fragmentShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_lambert_diffuse.frag");

		if (!vertexShaderFile.is_open() || !fragmentShaderFile.is_open())
			std::cerr << "ERROR: Failed to open shaders files!\n";
		else
			std::cout << "SUCCESS: Shaders files are open\n";

		std::string	vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
		std::string	fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());

		debugShaderProgram = Engine::GetInstance().GetGraphicsAPI().CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
		//glUseProgram(debugShaderProgram->GetProgramId());

		std::ifstream debugVertexShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_solid_color.vert");
		std::ifstream debugFragmentShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_solid_color.frag");

		if (!debugVertexShaderFile.is_open() || !debugFragmentShaderFile.is_open())
			std::cerr << "ERROR: Failed to open shaders files!\n";
		else
			std::cout << "SUCCESS: Shaders files are open\n";

		std::string debugVertexShaderSource((std::istreambuf_iterator<char>(debugVertexShaderFile)), std::istreambuf_iterator<char>());
		std::string debugFragmentShaderSource ((std::istreambuf_iterator<char>(debugFragmentShaderFile)), std::istreambuf_iterator<char>());

		debugColorShaderProgram = Engine::GetInstance().GetGraphicsAPI().CreateShaderProgram(debugVertexShaderSource, debugFragmentShaderSource);
		//glUseProgram(debugShaderProgram->GetProgramId());
	}

	void loadScreenSpaceShaders()
	{
		std::ifstream   ssVertexShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_screen_space.vert");
		std::ifstream   ssFragmentShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_screen_space.frag");

		if (!ssVertexShaderFile.is_open() || !ssFragmentShaderFile.is_open())
			std::cerr << "ERROR: Failed to open screen space shaders files!\n";
		else
			std::cout << "SUCCESS: Screen space shaders files are open\n";

		std::string	ssVertexShaderSource((std::istreambuf_iterator<char>(ssVertexShaderFile)), std::istreambuf_iterator<char>());
		std::string	ssFragmentShaderSource((std::istreambuf_iterator<char>(ssFragmentShaderFile)), std::istreambuf_iterator<char>());

		debugScreenSpaceShaderProgram = Engine::GetInstance().GetGraphicsAPI().CreateShaderProgram(ssVertexShaderSource, ssFragmentShaderSource);

		std::ifstream   compositeVertexShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_composite.vert");
		std::ifstream   compositeFragmentShaderFile("../engine/source/render/renderHelper/debuggingRenderAssets/debug_composite.frag");

		if (!compositeVertexShaderFile.is_open() || !compositeFragmentShaderFile.is_open())
			std::cerr << "ERROR: Failed to open shaders files!\n";
		else
			std::cout << "SUCCESS: Shaders files are open\n";

		std::string	compositeVertexShaderSource((std::istreambuf_iterator<char>(compositeVertexShaderFile)), std::istreambuf_iterator<char>());
		std::string	compositeFragmentShaderSource((std::istreambuf_iterator<char>(compositeFragmentShaderFile)), std::istreambuf_iterator<char>());

		debugCompositeShaderProgram = Engine::GetInstance().GetGraphicsAPI().CreateShaderProgram(compositeVertexShaderSource, compositeFragmentShaderSource);

	}

	void RenderQueue::Init()
	{
		auto& graphicsApi = Engine::GetInstance().GetGraphicsAPI();

		loadShaders();
		loadScreenSpaceShaders();
		sphereModel = renderhelper::loadModelFromOBJ("./assets/sphere.obj");
		std::cout << "The model: " << sphereModel->m_filename << " was loaded correctly \n";
		std::cout << "Materials loaded: " << sphereModel->m_materials.size() << "\n";

		constexpr	int		ELEMENTS_COUNT = 8;
		const		float	fullScreenQuadPos[ELEMENTS_COUNT] = {
			//		x		y
				-1.0f,   1.0f,		// v0
				-1.0f,  -1.0f,		// v1
				 1.0f,   1.0f,		// v2
				 1.0f,  -1.0f		// v3
		 };

		constexpr	int		QUAD_INDICES_SIZE = 6;
		unsigned	int		quadIndices[QUAD_INDICES_SIZE] =
		{
			0, 1, 2, // Triangle 1
			1, 3, 2	 // Tringle 2
		};

		VertexLayout vertexLayout;
		vertexLayout.elementsCount = ELEMENTS_COUNT;
		vertexLayout.elements = new VertexElement[ELEMENTS_COUNT]{};
		vertexLayout.elements[0] = {0,2,GL_FLOAT,0};
		vertexLayout.stride             = sizeof(float) * vertexLayout.elements[0].size;
		size_t totalFComponentsCount    = ELEMENTS_COUNT;
		debugScreenSpaceMesh = new Mesh(vertexLayout, &fullScreenQuadPos[0], totalFComponentsCount, &quadIndices[0], QUAD_INDICES_SIZE);

		sphereModelMatrix = scale(sphereModelMatrix, vec3(0.5f, 0.5f, 0.5f));

		///////////////////////////////////////////////////////////////
		/// Texture Generation
		///////////////////////////////////////////////////////////////
		{
			envmap_base_name = "sky";
			renderhelper::HDRImage image("./assets/hdr/" + envmap_base_name + ".hdr");
			// Generate handlers to the GPU resource
			glGenTextures(1, &environmentMap);
			// Bind the handler to set properties
			glBindTexture(GL_TEXTURE_2D, environmentMap);
			// Buffer the image data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.data);
			// Set the behaviour for sampling: magnification and minification, and out-of-bound sampling behaviour
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}



		///////////////////////////////////////////////////////////////////////////
		// Setup Framebuffers
		///////////////////////////////////////////////////////////////////////////
		//const int numFbos = 1;
		//for (size_t i = 0; i < numFbos; i++)
		//{ fboList.push_back(FrameBuffer(DEFALT_WINDOW_WIDTH, DEFALT_WINDOW_HEIGHT));}
		offScreenRenderTarget.InitializeFrameBuffer(DEFALT_WINDOW_WIDTH, DEFALT_WINDOW_HEIGHT);

	}



	void RenderQueue::Draw(GraphicsAPI& graphicsApi)
	{
		ActiveCamera& acticeCamera = Engine::GetInstance().GetMainCamera();
		const World& worldCoord = Engine::GetInstance().GetWorld();
		camera = acticeCamera.GetActiveCamera();
		const float cameraMoveSpeed = 5.0f;
		GLuint current_program = 0;
		GLuint uniCameraPos = 0;
		//////////////////////////////////////////////////////////////
		// Updating camera position
		//////////////////////////////////////////////////////////////

		auto& inputManager = Engine::GetInstance().GetInputManager();
		float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();

		if(inputManager.IsKeyPressed(GLFW_KEY_W))
		{
			camera->Position() += camera->Direction() * delta;
			std::cout << "Key[W]: Pressed! \n";
			std::cout << camera->Position().x << " camera.Pos.X\n";
			std::cout << camera->Position().y << " camera.Pos.Y\n";
			std::cout << camera->Position().z << " camera.Pos.Z\n";
		}


		if(inputManager.IsKeyPressed(GLFW_KEY_S))
		{
			camera->Position() += camera->Direction() * -delta;
			std::cout << "Key[S]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_D))
		{
			camera->Position() += camera->Right() * delta;
			std::cout << "Key[D]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_A))
		{
			camera->Position() -= camera->Right() * delta;
			std::cout << "Key[A]: Pressed! \n";
		}


		//////////////////////////////////////////////////////////////
		// Updating sphere position
		//////////////////////////////////////////////////////////////

		if(inputManager.IsKeyPressed(GLFW_KEY_UP))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos += worldCoord.worldForward * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_DOWN))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos -= worldCoord.worldForward * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_RIGHT))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos += worldCoord.worldRight * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_LEFT))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos -= worldCoord.worldRight * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		sphereModelMatrix[3].x = spherePos.x;
		sphereModelMatrix[3].y = spherePos.y;
		sphereModelMatrix[3].z = spherePos.z;

		const mat4		viewMatrix				= camera->ViewMatrix();
		const mat4		projectionMatrix		= camera->ProjectionMatrix();
		const vec3		camPosition				= camera->Position();

		//////////////////////////////////////////////////////////////
		/// Rendering starts  Set frame buffer
		//////////////////////////////////////////////////////////////
		offScreenRenderTarget.SetAsRenderTarget();
		//////////////////////////////////////////////////////////////
		/// FullScreen Quad
		//////////////////////////////////////////////////////////////
		//	glActiveTexture(GL_TEXTURE0); // GLSL 420
		// current_program = debugScreenSpaceShaderProgram->GetProgramId();
		glUseProgram(debugScreenSpaceShaderProgram->GetProgramId());

		// Get the uniforms' location
		uniCameraPos				= glGetUniformLocation(current_program, "camera_pos");
		const GLint uniEnvMultiplier			= glGetUniformLocation(current_program, "environment_multiplier");
		const GLint uniInvProjectionViewMatrix	= glGetUniformLocation(current_program, "inv_PV");
		const GLint uniEnvMap					= glGetUniformLocation(current_program, "environmentMap");
		// Set the values for the GPU
		glUniform3f(uniCameraPos, camPosition.x, camPosition.y, camPosition.z);
		glUniform1f(uniEnvMultiplier, environment_multiplier);

		mat4 invProjectionViewMatrix = inverse(projectionMatrix * viewMatrix);
		glUniformMatrix4fv(uniInvProjectionViewMatrix, 1, GL_FALSE, &invProjectionViewMatrix[0].x);

		//Set the texture units
		// GL_TEXTURE6 is a GLenum constant with value 33798 (0x84C6)
		// glUniform1i for a sampler expects the texture unit INDEX (0..15), not the enum
		// The shader looks for texture unit 33798 — which doesn't exist — so it returns black/zero
		glUniform1i(uniEnvMap, 6);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, environmentMap);
		// Use the program
		GLboolean depth_test_state;
		glGetBooleanv(GL_DEPTH_TEST, &depth_test_state);
		glDisable(GL_DEPTH_TEST);
		debugScreenSpaceMesh->Bind();
		debugScreenSpaceMesh->Draw();

		if (depth_test_state)
			glEnable(GL_DEPTH_TEST);

		glBindTexture(GL_TEXTURE_2D,0);
		glUseProgram(0);

		//////////////////////////////////////////////////////////////
		// TODO: Updating the uniforms of the debug shader
		//////////////////////////////////////////////////////////////
		//current_program = debugShaderProgram->GetProgramId(); // chick solution to get the debug_lambert_diffuse shader
		glUseProgram(debugShaderProgram->GetProgramId());
		//glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

		uniCameraPos				= glGetUniformLocation(current_program, "cameraPosition");
		const int uniModelMatrix	= glGetUniformLocation(current_program, "modelMatrix");
		const int uniProjectMatrix	= glGetUniformLocation(current_program, "projectionMatrix");
		const int uniLightDir		= glGetUniformLocation(current_program, "light_direction");

		const mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * sphereModelMatrix;

		glUniformMatrix4fv(uniModelMatrix,   1, false, &sphereModelMatrix[0].x);
		glUniformMatrix4fv(uniProjectMatrix, 1, false, &modelViewProjectionMatrix[0].x);
		glUniform3f(uniCameraPos, camPosition.x, camPosition.y, camPosition.z);

		glUniform3f(uniLightDir, gLightDir.x, gLightDir.y, gLightDir.z);

		renderhelper::render(sphereModel);

		// original for(auto& command : m_renderCommands)
		/*for (size_t i = 0; i < m_commandsCount; i++)
		{
			m_renderCommands[i].material->SetMaterialProjectionMatrix("projectionMatrix", projectionMatrix);
			m_renderCommands[i].material->SetCameraPosition("cameraPosition", cameraPosition);

			graphicsApi.BindMaterial( m_renderCommands[i].material );
			graphicsApi.BindMesh( m_renderCommands[i].mesh);

			graphicsApi.DrawMesh( m_renderCommands[i].mesh);

			m_usedCommands[i] = false;
		}

		// original m_renderCommands.clear() to clean up the std::vector<RenderCommand>
		m_commandsCount = 0;*/
		//glUseProgram(0);

		// --- In your main debug window ---


		///////////////////////////////////////////////////////////////////////////
		/// Draw Axis and Light
		///////////////////////////////////////////////////////////////////////////
		current_program = debugColorShaderProgram->GetProgramId();

		auto start = vec3(0.0f, 0.0f, 0.0f);
		auto end = vec3(10.0f, 0.0f, 0.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::RED);
		end = vec3(0.0f, 10.0f, 0.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::GREEN);
		end = vec3(0.0f, 0.0f, 10.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end, renderhelper::BLUE);
		end = vec3(10.f) * -gLightDir;
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::YELLOW);



		//glBindTexture(GL_TEXTURE_2D,0);
		//glUseProgram(0);

		/////

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, DEFALT_WINDOW_WIDTH, DEFALT_WINDOW_HEIGHT);
		glClearColor(1.f, 0.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////////////////////////////////////////////////////////
		/// Render to default frame buffer by sampling the scene buffer
		/////////////////////////////////////////////////////////////////
		glUseProgram(debugCompositeShaderProgram->GetProgramId());

		const GLuint uniSceneTexture = glGetUniformLocation(current_program, "sceneTexture");
		const GLuint uniDepthTexture = glGetUniformLocation(current_program, "depthTexture");


		glUniform1i(uniSceneTexture, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, offScreenRenderTarget.colorTexture);

		glUniform1i(uniSceneTexture, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, offScreenRenderTarget.depthTexture);

		// Set uniforms
		glGetBooleanv(GL_DEPTH_TEST, &depth_test_state);
		glDisable(GL_DEPTH_TEST);
		debugScreenSpaceMesh->Bind();
		debugScreenSpaceMesh->Draw();

		if (depth_test_state)
			glEnable(GL_DEPTH_TEST);

		glBindTexture(GL_TEXTURE_2D, 0);
		//ImGui::PushID("mag");

		ImGui::Separator();

		static int selectedCamera = 0;
		static bool showCameraWindow = false;

		const char* cameraNames[] = { "Camera 0", "Camera 1", "Camera 2" };
		ImGui::Text("Camera Selection");

		if (ImGui::BeginCombo("##camera", cameraNames[selectedCamera]))
		{
			for (int i = 0; i < 3; i++)
			{
				bool isSelected = (selectedCamera == i);
				if (ImGui::Selectable(cameraNames[i], isSelected))
					selectedCamera = i;
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// Button that toggles the separate camera window
		if (ImGui::Button("Camera Settings..."))
			showCameraWindow = !showCameraWindow;

		// ... rest of your debug window ...
		//ImGui::PopID();

		// --- Camera settings window, drawn OUTSIDE the main window ---
		if (showCameraWindow)
		{
			// Optional: set a default size the first time it appears
			ImGui::SetNextWindowSize(ImVec2(320, 300), ImGuiCond_FirstUseEver);

			// The bool* argument adds the X close button in the title bar
			ImGui::Begin("Camera Settings", &showCameraWindow);

			ImGui::Text("Active: %s", cameraNames[selectedCamera]);
			ImGui::Separator();

			/*ImGui::LabelText("Field of view",    "%.2f", pp.fov);
			ImGui::LabelText("Near plane",       "%.2f", pp.nearPlane);
			ImGui::LabelText("Far plane",        "%.2f", pp.farPlane);
			ImGui::LabelText("Viewport height",  "%d",   pp.height);
			ImGui::LabelText("Viewport width",   "%d",   pp.width);*/
			ImGui::LabelText("Camera Pos Z",     "%.2f", camera->Position().z);

			ImGui::Separator();
			if (ImGui::Button("Close"))
				showCameraWindow = false;

			ImGui::End();
		}
		// --- Light direction controls ---
		ImGui::Separator();

		ImGui::Text("Light Settings");

		// Option A: Three sliders — intuitive for direct axis control
		if (ImGui::SliderFloat("Light Dir X", &gLightDir.x, -1.0f, 1.0f) ||
			ImGui::SliderFloat("Light Dir Y", &gLightDir.y, -1.0f, 1.0f) ||
			ImGui::SliderFloat("Light Dir Z", &gLightDir.z, -1.0f, 1.0f))
		{
			// Re-normalize after any component changes so it stays a unit vector
			if (glm::length(gLightDir) > 0.0f)
				gLightDir = glm::normalize(gLightDir);
		}

		// Option B: SliderFloat3 — compact single-line alternative to Option A
		// if (ImGui::SliderFloat3("Light Direction", &lightDir.x, -1.0f, 1.0f))
		//     lightDir = glm::normalize(lightDir);

		// Live readout of the normalized result
		ImGui::LabelText("Normalized Dir", "(%.2f, %.2f, %.2f)",
						 gLightDir.x, gLightDir.y, gLightDir.z);

		if (ImGui::Button("Reset Light"))
			gLightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
		///////////////////////////////////////////////////////////////////////////
		// A button for reloading the shaders
		///////////////////////////////////////////////////////////////////////////
		ImGui::Separator();
		if(ImGui::Button("Reload Shaders"))
		{
			loadShaders();
			loadScreenSpaceShaders();
		}

		//ImGui::PopID();
	}
}
