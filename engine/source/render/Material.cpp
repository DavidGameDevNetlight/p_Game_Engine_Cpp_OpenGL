#include "Material.h"
#include "graphics/ShaderProgram.h"

namespace eng
{
	/// <summary>
	/// Takes in a pointer where the ShaderProgram is, and assings it to the Material
	/// Original parameter type: const std::shared_ptr<ShaderProgram>&
	/// </summary>
	void Material::SetShaderProgram(ShaderProgram* shaderProgram)
	{
		delete m_shaderProgram;				// Delete any old one
		m_shaderProgram = shaderProgram;	// Takes ownership of the memory
	}

	ShaderProgram* Material::GetShaderProgram() const
	{
		return m_shaderProgram;
	}

	void Material::SetMaterialProjectionMatrix(const string& matrixName, const mat4& matrix) const
	{
		if (!m_shaderProgram) return;
		m_shaderProgram->SetMatrix(matrixName,matrix);
	}

	void Material::SetCameraPosition(const string& cameraName, const vec3& position) const
	{
		if (!m_shaderProgram) return;
		m_shaderProgram->SetVector3(cameraName, position);
	}


	void Material::SetMaterialParameters(const string& paramName, float value)
	{
		m_parameters[paramName] = value;
	}

	void Material::Bind()
	{
		if (m_shaderProgram == nullptr)
			return;

		m_shaderProgram->Bind();
		for (const auto& iterator : m_parameters)
		{
			m_shaderProgram->SetFloat(iterator.first, iterator.second);
		}
	}

	Material::~Material()
	{
		delete m_shaderProgram;
		m_shaderProgram = nullptr;
	}
}