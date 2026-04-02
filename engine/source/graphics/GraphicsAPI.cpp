#include "GraphicsAPI.h"

#include "Engine.h"
#include "ShaderProgram.h"
#include "render/Material.h"
#include "render/Mesh.h"

namespace eng
{
	ShaderProgram* GraphicsAPI::CreateShaderProgram(const string& vertexSourceCode, const string& fragmentSourceCode)
	{

		/*
		//////////////////////////////
		Vertex
		//////////////////////////////
		Uniforms:
		modelMatrix
		projectionMatrix
		cameraPosition

		Layouts in:
		position
		normal

		out:
		ws_normals
		ndc_position (gl_Position)

		//////////////////////////////
		Fragment
		//////////////////////////////
		Uniforms:
		uColor
		material_color
		material_emission
		has_color_texture
		has_emission_texture

		Layouts bindings
		0 color_texture
		5 emission_texture

		Input (from vertex)
		ws_normal

		Output:
		FragColor
		 */

		//std::cout << fragmentSourceCode << std::endl;

		// Create handlers to shaders and program
		GLuint vertexShader			= glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader		= glCreateShader(GL_FRAGMENT_SHADER);
		GLuint shaderProgramID		= glCreateProgram();

		// Load the source code to the shader handler
		const char* vertexShaderSourceCode		= vertexSourceCode.c_str();
		const char* fragmentShaderSourceCode	= fragmentSourceCode.c_str();

		glShaderSource(vertexShader, 1, &vertexShaderSourceCode, nullptr);
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceCode, nullptr);

		// Compile the shader and validate
		glCompileShader(vertexShader);

		if (!IsShaderCompilationSuccessful(vertexShader, VERTEX))
		{
			DeletePrograms(vertexShader, fragmentShader, shaderProgramID);
			return nullptr;
		}

		glCompileShader(fragmentShader);
		if (!IsShaderCompilationSuccessful(fragmentShader, FRAGMENT))
		{
			DeletePrograms(vertexShader, fragmentShader, shaderProgramID);
			return nullptr;
		}

		// Attach to shader program
		glAttachShader(shaderProgramID, vertexShader);
		glAttachShader(shaderProgramID, fragmentShader);
		
		// Link shader program and validate
		glLinkProgram(shaderProgramID);
		if (!IsProgramLinkingSuccessful(shaderProgramID))
		{
			DeletePrograms(vertexShader, fragmentShader, shaderProgramID);
			return nullptr;
		}

		// Delete shader handlers
		DeletePrograms(vertexShader, fragmentShader);

		// original return type: std::make_shared<ShaderProgram>(shaderProgramID)
		// Read more about the learnings on this on Session notes Feb 19 2026
		// TODO: Validate this following: GraphicsAPI creates memory in the heap, but does not owns it.
		// someone else has to take it and deal with the deletion
		return new ShaderProgram(shaderProgramID);
	}

	void GraphicsAPI::BindShaderProgram(ShaderProgram* shaderProgram)
	{
		if(shaderProgram != nullptr)
			shaderProgram->Bind();
	}

	void GraphicsAPI::BindMaterial(Material* material)
	{
		// No nullptr check is needed because we are passing a ref.
		//material.Bind();

		if (material != nullptr)
			material->Bind();
	}


	void GraphicsAPI::BindMesh(Mesh* mesh)
	{
		// No nullptr check is needed because we are passing a ref.
		//mesh.Bind();
		
		if (mesh != nullptr)
			mesh->Bind();
	}

	GLuint GraphicsAPI::CreateVertexBuffer(const float* vertices, const size_t totalFComponentsCount)
	{
		GLuint vbo = 0;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);              
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalFComponentsCount, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vbo;
		// Note a common mistake, Passing vertices as &vertices, sends the address of a stack variable
		// to the GPU, instead of the pointer to the firts geometry data point in the array.
		// But, passing vertices = &vertives[0], because this is the address of the first element in the array
	}

	GLuint GraphicsAPI::CreateIndexBuffer(const unsigned int* indices, const size_t indicesCount)
	{
		GLuint ebo = 0;

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return ebo;

	}

	void GraphicsAPI::SetClearColor(const float r, const float g, const float b, const float a)
	{
		glClearColor(r,g,b,a);
	}
	void GraphicsAPI::ClearBuffers()
	{
		//glViewport(0, 0, DEFALT_WINDOW_WIDTH, DEFALT_WINDOW_HEIGHT);                             // Set viewport
		// Clears the color buffer and the z-buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

	}

	void GraphicsAPI::DrawMesh(Mesh* mesh)
	{
		if(mesh != nullptr)
			mesh->Draw();
	}

	bool GraphicsAPI::IsShaderCompilationSuccessful(const GLuint shader, ShaderType type) const
	{
		GLint success;

		constexpr unsigned int LOG_BUFFER_SIZE = 512;
		char infoLog[LOG_BUFFER_SIZE];

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, LOG_BUFFER_SIZE, nullptr, infoLog);
			std::cerr << "ERROR:"
						<< (type == VERTEX ? "VERTEX" : "FRAGMENT")
						<<" SHADER COMPILATION FAILED " << infoLog << '\n';
			return false;
		}

		return true;
	}

	bool GraphicsAPI::IsProgramLinkingSuccessful(const GLuint program) const
	{
		GLint success;
		constexpr unsigned int LOG_BUFFER_SIZE = 512;
		char infoLog[LOG_BUFFER_SIZE];

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		
		if (!success)
		{
			glGetProgramInfoLog(program, LOG_BUFFER_SIZE, nullptr, infoLog);
			std::cerr << "ERROR: SHADER PROGRAM LINKING FAILED " << infoLog << '\n';
			return false;
		}

		return true;
	}

	void GraphicsAPI::DeletePrograms(const GLuint vertShader, const GLuint fragShader, const GLuint shaderProgram)
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(shaderProgram);
	}
	void GraphicsAPI::DeletePrograms(const GLuint vertShader, const GLuint fragShader)
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}


}