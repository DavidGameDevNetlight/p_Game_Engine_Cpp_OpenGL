#include <Game.h>
#include <GLFW/glfw3.h>
#include "eng.h"
using eng::Engine;

bool Game::Init()
{
    //////////////////////////////////////////////////////////////
    // Shader sources
    //////////////////////////////////////////////////////////////
    std::string vertexShaderSource = R"(
        #version 330 core
        
        uniform vec3 uOffSet;

        uniform mat4 projectionMatrix;
        uniform vec3 cameraPosition;

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;
        
        out vec3 colorUV;

        void main()
        {
            // This is a hardcoded rotation matrix we use to have the camera look slightly downward
	        mat3 V = mat3(
	        1.0, 0.0, 0.0,
	        0.0, 0.97, 0.26,
	        0.0, -0.26, 0.97);

	        vec4 pos = vec4((V * position.xyz) - cameraPosition.xyz, 1);
	        gl_Position = projectionMatrix * pos;

            //gl_Position = vec4( position.x, position.y, position.z, 1.0);
            //gl_Position = vec4( position.x + uOffSet.x , position.y + uOffSet.y, 0.0, 1.0);

            colorUV = color;
        }    

    )";

    std::string fragmentShaderSource = R"(
        #version 330 core
        
        uniform vec4 uColor;
        
        in vec3 colorUV;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(colorUV.r, colorUV.g, colorUV.b, 1.0);
            //FragColor = vec4(uColor.r, uColor.g, uColor.b, 1.0);
        }
    )";

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

	return true;
}

void Game::Update(float deltaTime)
{
	//std::cout << "Delta time: " << deltaTime << "\n";
	
	auto& inputManager = Engine::GetInstance().GetInputManager();
	if(inputManager.IsKeyPressed(GLFW_KEY_A))
		std::cout << "Key[A]: Pressed! \n";

    eng::RenderCommand command;
    command.material    = &m_material;
    command.mesh        = m_mesh; // original m_mesh.get() to get from std::unique_ptr<eng::Mesh>

    auto& renderQueue = Engine::GetInstance().GetRenderQueue();
    renderQueue.Submit(command);
}

void Game::Destroy()
{
    delete m_mesh;
    m_mesh = nullptr;
}