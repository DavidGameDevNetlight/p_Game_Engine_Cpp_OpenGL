#include "render/RenderQueue.h"

#include "glm/fwd.hpp"
#include "graphics/GraphicsAPI.h"
#include "render/Material.h"
#include "render/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include "GLFW/glfw3.h"
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
	//vec3 cameraPosition	= vec3(0.f,10.f,0.f);
	void RenderQueue::Draw(GraphicsAPI& graphicsApi)
	{
		// TODO: Adding a 3D camera by setting up the projection matrix
		const float cameraMoveSpeed = 5.0f;
		const vec3	cameraRight		= normalize(cross(cameraDirection, worldUp));
		const vec3	cameraUp		= normalize(cross(cameraRight, cameraDirection));

		const mat3	cameraBaseVectorWorldSpace(cameraRight, cameraUp, -cameraDirection);
		// This allows to rotate the vertices of models based on the camera
		mat4 cameraRotation = transpose(cameraBaseVectorWorldSpace);
		const float aspectRatio	= static_cast<float>(pp.width) / static_cast<float>(pp.height);
		const mat4 projectionMatrix = perspective(radians(pp.fov), aspectRatio, pp.nearPlane, pp.farPlane);

		//////////////////////////////////////////////////////////////
		// TODO: Updating camera poisiton
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
	}
}
