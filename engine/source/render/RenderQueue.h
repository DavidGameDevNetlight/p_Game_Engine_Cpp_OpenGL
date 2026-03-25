#pragma once

#include <cstddef>

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

	class RenderQueue
	{
	public:
		
		/// <summary>
		/// 
		/// </summary>
		~RenderQueue();

		// original const RenderComman& commandd
		void Submit(const RenderCommand& command);
		void Draw(GraphicsAPI& graphicsApi);

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