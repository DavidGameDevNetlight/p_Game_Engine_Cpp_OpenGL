#include "render/Mesh.h"
#include "Engine.h"

namespace eng
{
	Mesh::Mesh(	const VertexLayout&		layout,
				const float*			vertices,
				const size_t			totalFComponentsCount,
				const unsigned int*		indices,
				const size_t			indicesCount)
	{
		// Deep copy is needed, the = operator and the copy constructor will be called
		// Without a deep copy, layout and m_vertexlayout will point to the same memory address
		// This assignation calls alot of things implicitly in the std::vector class, reaad Session Notes Feb 21 2026
		m_vertexLayout = layout; 

		auto& graphicsAPI = Engine::GetInstance().GetGraphicsAPI();

		m_VBO = graphicsAPI.CreateVertexBuffer(vertices, totalFComponentsCount);
		m_EBO = graphicsAPI.CreateIndexBuffer(indices, indicesCount);

		glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		// Loop over the array of elements in the vertex layout

		for (size_t i = 0; i < layout.elementsCount; i++)
		{
			// Define how to interprete the data
			glVertexAttribPointer(	layout.elements[i].index, 
									layout.elements[i].size,
									layout.elements[i].type,
									GL_FALSE,
									layout.stride,
									(void*) layout.elements[i].offset);

			glEnableVertexAttribArray(layout.elements[i].index);

		}

		// The VAP records this EBO binding, stores internally
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO); 
		// The VAO internally 

		// Unbinding
		glBindVertexArray(0); // VAO is "frozen", still remembers EBO.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // If the EBO in unbounded before the VAO, the VAO will record 0.

		m_vertexCount = (totalFComponentsCount * sizeof(float)) / m_vertexLayout.stride;
		m_indexCount = indicesCount;
	}

	Mesh::Mesh(	const VertexLayout&		layout,
				const float*			vertices,
				const size_t			vertexElementSize)
	{
		m_vertexLayout = layout;

		auto& graphicsAPI = Engine::GetInstance().GetGraphicsAPI();

		m_VBO = graphicsAPI.CreateVertexBuffer(vertices, vertexElementSize);

		glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		// Loop over the array of elements in the vertex layout

		for (size_t i = 0; i < layout.elementsCount; i++)
		{
			// Define how to interprete the data
			glVertexAttribPointer(layout.elements[i].index,
				layout.elements[i].size,
				layout.elements[i].type,
				GL_FALSE,
				layout.stride,
				(void*) layout.elements[i].offset);

			glEnableVertexAttribArray(layout.elements[i].index);

		}

		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_vertexCount = vertexElementSize;//(verticesCount * sizeof(float)) / m_vertexLayout.stride;

	}

	Mesh::~Mesh()
	{
		DeleteBuffers();
	}

	void Mesh::Bind()
	{
		glBindVertexArray(m_VAO);
	}

	void Mesh::Draw()
	{
		if (m_indexCount > 0)
			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);

		glUseProgram(0);
	}

	void Mesh::DeleteBuffers()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		if (m_EBO != 0)
			glDeleteBuffers(1, &m_EBO);
	}
}