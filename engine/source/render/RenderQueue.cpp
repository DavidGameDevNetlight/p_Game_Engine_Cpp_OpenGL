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
using namespace glm;

namespace eng
{
	//TODO: all of these are temporal debug variables that will be moved
	vec3 gLightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
	renderhelper::Model* sphereModel = nullptr;
	ShaderProgram* debugShaderProgram = nullptr;
	ShaderProgram* debugScreenSpaceShaderProgram = nullptr;
	Mesh* debugScreenSpaceMesh = nullptr;

	vec3 spherePos = vec3(0.0f);

	mat4 sphereModelMatrix(	1.0f, 0.0f, 0.0f, 0.0f, // x
							0.0f, 1.0f, 0.0f, 0.0f, // y
							0.0f, 0.0f, 1.0f, 0.0f, // z
							0.0f, 0.0f, 0.0f, 1.0f); // translation


	RenderQueue::~RenderQueue()
	{
		// For the record, this is a commun mistake, a raw array is NOT heap allocated unless it
		// was created using new. Even if what it stores are pointers to heap allocated memory
		// delete[] m_renderCommands;
		delete debugScreenSpaceMesh;
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
		glUseProgram(debugShaderProgram->GetProgramId());
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

	}



	void RenderQueue::Draw(GraphicsAPI& graphicsApi)
	{
		// TODO: Adding a 3D camera by setting up the projection matrix
		const float cameraMoveSpeed = 5.0f;
		const vec3	cameraRight		= normalize(cross(cameraDirection, worldUp));
		const vec3	cameraUp		= normalize(cross(cameraRight, cameraDirection));

		const mat3		cameraBaseVectorWorldSpace(cameraRight, cameraUp, -cameraDirection);
		// This allows to rotate the vertices of models based on the camera

		const mat4		cameraRotation			= transpose(cameraBaseVectorWorldSpace);
		const float		aspectRatio				= static_cast<float>(pp.width) / static_cast<float>(pp.height);

		//////////////////////////////////////////////////////////////
		// Updating camera position
		//////////////////////////////////////////////////////////////

		auto& inputManager = Engine::GetInstance().GetInputManager();
		if(inputManager.IsKeyPressed(GLFW_KEY_W))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			cameraPosition += cameraDirection * delta;
			std::cout << "Key[W]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_S))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			cameraPosition += cameraDirection * -delta;
			std::cout << "Key[S]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_D))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			cameraPosition += cameraRight * delta;
			std::cout << "Key[D]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_A))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			cameraPosition -= cameraRight * delta;
			std::cout << "Key[A]: Pressed! \n";
		}

		//////////////////////////////////////////////////////////////
		// Updating sphere position
		//////////////////////////////////////////////////////////////

		if(inputManager.IsKeyPressed(GLFW_KEY_UP))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos += worldForward * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_DOWN))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos -= worldForward * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_RIGHT))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos += worldRight * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		if(inputManager.IsKeyPressed(GLFW_KEY_LEFT))
		{
			float delta = cameraMoveSpeed * Engine::GetInstance().GetDeltaTime();
			spherePos -= worldRight * delta;
			std::cout << "Key[UP]: Pressed! \n";
		}

		//sphereModelMatrix = translate(spherePos);
		sphereModelMatrix[3].x = spherePos.x;
		sphereModelMatrix[3].y = spherePos.y;
		sphereModelMatrix[3].z = spherePos.z;


		// The negative cameraPosition makes the camera back to the origin.
		// The viewMatrix is in reverse order, we first translate back to the origin and then rotate
		const mat4		viewMatrix				= cameraRotation * translate(-cameraPosition);
		const mat4		projectionMatrix		= perspective(radians(pp.fov), aspectRatio, pp.nearPlane, pp.farPlane);

		//////////////////////////////////////////////////////////////
		/// FullScreen Quad
		//////////////////////////////////////////////////////////////

		GLboolean depth_test_state;
		glGetBooleanv(GL_DEPTH_TEST, &depth_test_state);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(debugScreenSpaceShaderProgram->GetProgramId());
		debugScreenSpaceMesh->Bind();
		debugScreenSpaceMesh->Draw();

		if (depth_test_state)
			glEnable(GL_DEPTH_TEST);

		//////////////////////////////////////////////////////////////
		// TODO: Updating the uniforms of the debug shader
		//////////////////////////////////////////////////////////////
		const GLuint current_program = debugShaderProgram->GetProgramId(); // chick solution to get the debug_lambert_diffuse shader
		//glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
		glUseProgram(current_program);

		const int uniModelMatrix	= glGetUniformLocation(current_program, "modelMatrix");
		const int uniProjectMatrix	= glGetUniformLocation(current_program, "projectionMatrix");
		const int uniCameraPos		= glGetUniformLocation(current_program, "cameraPosition");
		const int uniLightDir		= glGetUniformLocation(current_program, "light_direction");

		const mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * sphereModelMatrix;
		glUniformMatrix4fv(uniModelMatrix,   1, false, &sphereModelMatrix[0].x);
		//glUniformMatrix4fv(uniProjectMatrix, 1, false, &modelViewProjectionMatrix[0].x);
		glUniformMatrix4fv(uniProjectMatrix, 1, false, &modelViewProjectionMatrix[0].x);

		glUniform3f(uniCameraPos, cameraPosition.x, cameraPosition.y, cameraPosition.z);
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

			ImGui::LabelText("Field of view",    "%.2f", pp.fov);
			ImGui::LabelText("Near plane",       "%.2f", pp.nearPlane);
			ImGui::LabelText("Far plane",        "%.2f", pp.farPlane);
			ImGui::LabelText("Viewport height",  "%d",   pp.height);
			ImGui::LabelText("Viewport width",   "%d",   pp.width);
			ImGui::LabelText("Camera Pos Z",     "%.2f", cameraPosition.z);

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
		}

		//ImGui::PopID();

		///////////////////////////////////////////////////////////////////////////
		/// Draw Axis and Light
		///////////////////////////////////////////////////////////////////////////
		auto start = vec3(0.0f, 0.0f, 0.0f);
		auto end = vec3(10.0f, 0.0f, 0.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::RED);
		end = vec3(0.0f, 10.0f, 0.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::GREEN);
		end = vec3(0.0f, 0.0f, 10.0f);
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end, renderhelper::BLUE);
		end = vec3(10.f) * -gLightDir;
		renderhelper::drawLineSegment( current_program, viewMatrix, projectionMatrix, start, end,renderhelper::YELLOW);

	}
}
