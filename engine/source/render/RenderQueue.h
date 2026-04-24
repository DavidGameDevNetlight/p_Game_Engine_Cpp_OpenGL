#pragma once
#include <cstddef>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

using namespace glm;

namespace eng
{
	class Mesh;
	class Material;
	class GraphicsAPI;

	class Camera;

	struct RenderCommand
	{
		Mesh*		mesh			= nullptr;
		Material*	material		= nullptr;
		mat4*		modelMatrix		= nullptr;
	};

	//////////////////////////////////////////////////////////////
	// Temporal public variables
	//////////////////////////////////////////////////////////////
	struct FrameBuffer
	{
		/*unsigned int*/GLuint 	frameBufferId	= 0;
		/*unsigned int*/GLuint	colorTexture	= 0;
		/*unsigned int*/GLuint	depthTexture	= 0;
		int	width			= 0;
		int	height			= 0;
		bool			isCompleted		= false;
		FrameBuffer():
		frameBufferId(0),
		colorTexture(0),
		depthTexture(0),
		width(0),
		height(0),
		isCompleted(false){}
		~FrameBuffer();
		void InitializeFrameBuffer(const int viewPortWidth, const int viewPortHeight);
		void GenerateColorBuffer(const int viewPortWidth, const int viewPortHeight);
		void GenerateDepthBuffer(const int viewPortWidth, const int viewPortHeight);
		void resize(const int viewPortWidth, const int viewPortHeight);
		void SetAsRenderTarget() const;
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
		Camera* camera = nullptr;

		//////////////////////////////////////////////////////////////
		/// Framebuffers management
		//////////////////////////////////////////////////////////////
		void ResizeRenderTargets(const int newPixelsWidth, const int newPixelsHeight);

	private:
		static const size_t RENDER_COMMANDS_SIZE = 100;

		/// Stack allocated fixed-sized array, no need for manual deletion
		/// Original memeber type: std::vector<RenderCommand>
		/// TODO: this one is tricky, because it may grow dynamically, for now, it will be stack allocated
		RenderCommand	m_renderCommands[RENDER_COMMANDS_SIZE]	= {};
		bool			m_usedCommands[RENDER_COMMANDS_SIZE]	= {};
		size_t			m_commandsCount							= 0;

		//////////////////////////////////////////////////////////////
		/// Framebuffers management
		//////////////////////////////////////////////////////////////
		static constexpr	size_t FRAME_BUFFERS_COUNT			= 1;
		FrameBuffer*		m_frameBuffers						= nullptr;
	};
}

/**
Initialily I was using
RenderCommand*	m_renderCommands[RENDER_COMMANDS_SIZE]	= { nullptr };
For an array that holds pointers, those pointers are NON-OWNING, RenderQueue does not own them

*/