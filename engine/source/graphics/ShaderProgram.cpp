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

	void  ShaderProgram::SetUniformFloat(const string& uniformName, float value)
	{
		auto uniformLocation = GetUniformLocation(uniformName);
		glUniform1f(uniformLocation, value);
	}

	void  ShaderProgram::SetUniform3Float(const string& uniformName, vec3 vector)
	{
		auto uniformLocation = GetUniformLocation(uniformName);
		glUniform3f(uniformLocation, vector.x, vector.y, vector.z);
	}


	void ShaderProgram::SetProjectionMatrix(const string& matrixName, mat4 matrix)
	{
		auto uniformLocation = GetUniformLocation(matrixName);
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &matrix[0].x);
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