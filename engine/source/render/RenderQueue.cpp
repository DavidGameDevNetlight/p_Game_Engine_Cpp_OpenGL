#include "render/RenderQueue.h"

#include "glm/fwd.hpp"
#include "graphics/GraphicsAPI.h"
#include "render/Material.h"
#include "render/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
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
	vec3 cameraPosition	= vec3(0.f,10.f,0.f);
	void RenderQueue::Draw(GraphicsAPI& graphicsApi)
	{
		// TODO: Adding a 3D camera by setting up the projection matrix
		float w = 1280; // TODO: make it dynamic
		float h = 720;

		float fovy			= radians(45.0f);
		float aspectRatio	= w / h;
		float nearPlaner	= 0.01;
		float farPlaner		= 400.f;
		mat4 projectMatrix	= perspective(fovy, aspectRatio, nearPlaner, farPlaner);

		// original for(auto& command : m_renderCommands)
		for (size_t i = 0; i < m_commandsCount; i++)
		{
			m_renderCommands[i].material->SetMaterialProjectionMatrix("projectionMatrix", projectMatrix);
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
