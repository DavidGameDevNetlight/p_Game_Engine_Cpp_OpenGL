#include <Game.h>
#include <GLFW/glfw3.h>
#include "eng.h"
#include <fstream>
#include <streambuf>
using eng::Engine;

bool Game::Init()
{
    //////////////////////////////////////////////////////////////
    // Shader sources
    //////////////////////////////////////////////////////////////
    /*
    std::ifstream   vertexShaderFile("./shaders/debug_lambert_diffuse.vert");
    std::ifstream   fragmentShaderFile("./shaders/debug_lambert_diffuse.frag");

    if (!vertexShaderFile.is_open() || !fragmentShaderFile.is_open())
        std::cerr << "ERROR: Failed to open shaders files!\n";

    std::string     vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
    std::string     fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());

    // First time the GraphicsAPI gets instantiated

    auto& graphicsApi = Engine::GetInstance().GetGraphicsAPI();

    //original auto
    eng::ShaderProgram* shaderProgram = graphicsApi.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    m_material.SetShaderProgram(shaderProgram);

    const unsigned int  RECT_VERTICES_DATA_SIZE = 24, RECT_INDICES_SIZE = 6;
    const size_t        POSITION = 0, COLOR = 1;
    constexpr int       ELEMENTS_COUNT = 2;

    float rectangleVertices[RECT_VERTICES_DATA_SIZE] =
    {
         0.0f,     0.5f,  -40.0f,   1.0f,   0.0f,    0.0f,
         0.0f,   -10.0f,  -40.0f,   0.0f,   1.0f,    0.0f,
        10.0f,   -10.0f,  -40.0f,   0.0f,   0.0f,    1.0f,
        10.5f,     0.0f,  -40.0f,   1.0f,   1.0f,    0.0f

    };

    unsigned int rectangleIndices[RECT_INDICES_SIZE] =
    {
        0, 1, 2,
        0, 2, 3
    };

    eng::VertexLayout vertexLayout;
    vertexLayout.elementsCount = ELEMENTS_COUNT;
    vertexLayout.elements = new eng::VertexElement[ELEMENTS_COUNT]{};

    //                                    index  size  type           offset
    vertexLayout.elements[POSITION] = {     0,    3,  GL_FLOAT,         0          };
    vertexLayout.elements[COLOR]    = {     1,    3,  GL_FLOAT, sizeof(float) * 3  };

    vertexLayout.stride             = sizeof(float) * (vertexLayout.elements[0].size + vertexLayout.elements[1].size);
    size_t totalFComponentsCount    = RECT_VERTICES_DATA_SIZE;

    m_mesh = new eng::Mesh(vertexLayout, &rectangleVertices[0], totalFComponentsCount, &rectangleIndices[0], RECT_INDICES_SIZE);
    */
	return true;
}

void Game::Update(float deltaTime)
{
	//std::cout << "Delta time: " << deltaTime << "\n";
	/*
	auto& inputManager = Engine::GetInstance().GetInputManager();
	if(inputManager.IsKeyPressed(GLFW_KEY_A))
		std::cout << "Key[A]: Pressed! \n";

    eng::RenderCommand command;
    command.material    = &m_material;
    command.mesh        = m_mesh; // original m_mesh.get() to get from std::unique_ptr<eng::Mesh>

    auto& renderQueue = Engine::GetInstance().GetRenderQueue();
    renderQueue.Submit(command);
    */
}

void Game::Destroy()
{
    /*
    delete m_mesh;
    m_mesh = nullptr;
    */
}