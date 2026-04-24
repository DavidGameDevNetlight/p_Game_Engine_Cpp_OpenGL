#include "graphics/ShaderProgram.h"

namespace eng
{
	ShaderProgram::ShaderProgram(GLuint shaderProgramId) : m_shaderProgramId(shaderProgramId)
	{

	};

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_shaderProgramId);
	};

	const GLuint ShaderProgram::GetProgramId() const{
		return m_shaderProgramId;
	}

	void ShaderProgram::Bind() const
	{
		glUseProgram(m_shaderProgramId);
	}

	void  ShaderProgram::SetFloat(const string& uniformName, float value)
	{
		const auto uniformLocation = GetUniformLocation(uniformName);
		glUniform1f(uniformLocation, value);
	}

	void  ShaderProgram::SetInt(const string& uniformName, int value)
	{
		const auto uniformLocation = GetUniformLocation(uniformName);
		glUniform1i(uniformLocation, value);
	}

	void  ShaderProgram::SetVector3(const string& uniformName, vec3 vector)
	{
		const auto uniformLocation = GetUniformLocation(uniformName);
		glUniform3f(uniformLocation, vector.x, vector.y, vector.z);
	}

	void ShaderProgram::SetMatrix(const string& matrixName, mat4 matrix)
	{
		const auto uniformLocation = GetUniformLocation(matrixName);
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &matrix[0].x);
	}

	void ShaderProgram::SetCameraPosition(const string& cameraName, vec3 position)
	{
		const auto uniformLocation = GetUniformLocation(cameraName);
		glUniform3f(uniformLocation, position.x, position.y, position.z);
	}

	GLint ShaderProgram::GetUniformLocation(const string& uniformName)
	{
		auto iterator = m_uniformLocationCache.find(uniformName);
		
		if (iterator != m_uniformLocationCache.end())
			return iterator->second;

		GLint uniformLocation = glGetUniformLocation(m_shaderProgramId, uniformName.c_str());
		m_uniformLocationCache[uniformName] = uniformLocation;
		return uniformLocation;
	}


}