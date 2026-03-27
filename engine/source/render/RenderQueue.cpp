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
#include "renderHelper/Model.h"
using namespace glm;

namespace eng
{

	RenderQueue::~RenderQueue()
	{
		// For the record, this is a commun mistake, a raw array is NOT heap allocated unless it
		// was created using new. Even if what it stores are pointers to heap allocated memory
		// delete[] m_renderCommands;
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

	rederhelper::Model* sphereModel = nullptr;
	mat4 sphereModelMatrix(	1.0f, 0.0f, 0.0f, 0.0f, // x
							0.0f, 1.0f, 0.0f, 0.0f, // y
							0.0f, 0.0f, 1.0f, 0.0f, // z
							1.0f, 5.0f, 1.0f, 1.0f); // translation

	void RenderQueue::Init()
	{
		auto& graphicsApi = Engine::GetInstance().GetGraphicsAPI();

		//////////////////////////////////////////////////////////////
		// Shader sources for debugging
		//////////////////////////////////////////////////////////////

		std::ifstream   vertexShaderFile("./assets/debug_lambert_diffuse.vert");
		std::ifstream   fragmentShaderFile("./assets/debug_lambert_diffuse.frag");

		if (!vertexShaderFile.is_open() || !fragmentShaderFile.is_open())
			std::cerr << "ERROR: Failed to open shaders files!\n";

		std::string     vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
		std::string     fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());


		eng::ShaderProgram* shaderProgram = graphicsApi.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
		sphereModel = rederhelper::loadModelFromOBJ("./assets/sphere.obj");
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

		// The negative cameraPosition makes the camera back to the origin.
		// The viewMatrix is in reverse order, we first translate back to the origin and then rotate
		const mat4		viewMatrix				= cameraRotation * translate(-cameraPosition);
		const mat4		projectionMatrix		= perspective(radians(pp.fov), aspectRatio, pp.nearPlane, pp.farPlane);

		//////////////////////////////////////////////////////////////
		// TODO: Sphere
		//////////////////////////////////////////////////////////////
		GLint current_program = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

		const int mvploc	= glGetUniformLocation(current_program, "projectionMatrix");
		const int mloc		= glGetUniformLocation(current_program, "modelMatrix");

		const mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * sphereModelMatrix;
		glUniformMatrix4fv(mvploc, 1, false, &modelViewProjectionMatrix[0].x);
		glUniformMatrix4fv(mloc, 1, false, &sphereModelMatrix[0].x);
		rederhelper::render(sphereModel);

		//////////////////////////////////////////////////////////////
		// TODO: Updating camera position
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

		// original for(auto& command : m_renderCommands)
		for (size_t i = 0; i < m_commandsCount; i++)
		{
			m_renderCommands[i].material->SetMaterialProjectionMatrix("projectionMatrix", projectionMatrix);
			m_renderCommands[i].material->SetCameraPosition("cameraPosition", cameraPosition);

			graphicsApi.BindMaterial( m_renderCommands[i].material );
			graphicsApi.BindMesh( m_renderCommands[i].mesh);

			graphicsApi.DrawMesh( m_renderCommands[i].mesh);

			m_usedCommands[i] = false;
		}

		// original m_renderCommands.clear() to clean up the std::vector<RenderCommand>
		m_commandsCount = 0;

		ImGui::PushID("mag");
		ImGui::Text("Camera setting");
		ImGui::LabelText("Field of view", "%.2f", pp.fov);
		ImGui::LabelText("Near plance", "%.2f", pp.nearPlane);
		ImGui::LabelText("Far plane", "%.2f", pp.farPlane);
		ImGui::LabelText("Viewport height", "%d", pp.height);
		ImGui::LabelText("Viewport width", "%d", pp.width);
		ImGui::PopID();
	}
}
