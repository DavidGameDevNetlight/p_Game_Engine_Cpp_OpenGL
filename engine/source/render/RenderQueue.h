#pragma once
#include <cstddef>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

	using namespace glm;
namespace eng
{
	class Mesh;
	class Material;
	class GraphicsAPI;

	// TODO print the size of one RenderCommand, it should be 16 bytes, as it container only 2 pointers
	struct RenderCommand
	{
		Mesh*		mesh		= nullptr;
		Material*	material	= nullptr;
	};

	//////////////////////////////////////////////////////////////
	// Temporal public variables
	//////////////////////////////////////////////////////////////
	struct PerspectiveParams
	{
		float	fov;
		int		width;
		int		height;
		float	nearPlane;
		float	farPlane;
	};

	class RenderQueue
	{
	public:
		
		/// <summary>
		///
		/// </summary>
		~RenderQueue();

		// TODO: Temporarily adding an Init for Debug meshes
		void Init();

		// original const RenderComman& commandd
		void Submit(const RenderCommand& command);
		void Draw(GraphicsAPI& graphicsApi);

		//////////////////////////////////////////////////////////////
		// Temporal public variables
		//////////////////////////////////////////////////////////////
		vec3	worldUp				= vec3(0.0f, 1.0f, 0.0f);
		vec3	worldRight			= vec3(1.0f, 0.0f, 0.0f);
		vec3	worldForward		= vec3(0.0f, 0.0f, 1.0f);

		ivec2	prevMouseCoords 	= { -1, -1 };
		bool	isMouseDragging 	= false;
		// Camera params
		vec3	cameraPosition 		= vec3(15.0f, 15.0f, 15.0f);
		vec3	cameraDirection		= vec3(-1.0f, -1.0f, -1.0f);
		mat4	T					= mat4(1.0f);
		mat4	R					= mat4(1.0f);

		PerspectiveParams	pp		= { 45.0f, 1280, 720, 0.1f, 300.0f };
		int 				old_w 	= 1280;
		int 				old_h 	= 720;

	private:
		static const size_t RENDER_COMMANDS_SIZE = 100;

		/// Stack allocated fixed-sized array, no need for manual deletion
		/// Original memeber type: std::vector<RenderCommand>
		/// TODO: this one is tricky, because it may grow dynamically, for now, it will be stack allocated
		RenderCommand	m_renderCommands[RENDER_COMMANDS_SIZE]	= {};
		bool			m_usedCommands[RENDER_COMMANDS_SIZE]	= {};
		size_t			m_commandsCount							= 0;

	};
}

/**
Initialily I was using
RenderCommand*	m_renderCommands[RENDER_COMMANDS_SIZE]	= { nullptr };
For an array that holds pointers, those pointers are NON-OWNING, RenderQueue does not own them

*/