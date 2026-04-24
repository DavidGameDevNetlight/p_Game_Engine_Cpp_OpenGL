#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
using namespace glm;

using std::unordered_map;
using std::string;

namespace eng
{
	class ShaderProgram
	{
	public: 
		// Disable default means that the a user will not be able to create a ShaderProgram by just declaring an instance.
		// Calling `ShaderPragram sp; will yield an error. And assinging to an `int` will not work. This ensures the program id
		// comes from a valid OpenGL action on the GPU.
		ShaderProgram() = delete;
		
		// Disable copy constructor and operator
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator= (const ShaderProgram&) = delete;
		
		/// <summary>
		/// This is an explicit constructor that does not allow for implicit cast from GLuint to ShaderProgram.
		/// Therefore, only by explicitly creating a GLuint handler a ShaderProgram instance can be instantiated.
		/// </summary>
		explicit ShaderProgram(GLuint programId);
		~ShaderProgram();
		
		void Bind() const;

		//////////////////////////////////////////////////////////////
		// Setting Uniforms in the shaders
		//////////////////////////////////////////////////////////////
		void SetFloat(				const string& uniformName,	float	value);
		void SetInt(				const string& uniformName,	int		value);
		void SetVector3(			const string& uniformName,	vec3	value);
		void SetMatrix(				const string& matrixName,	mat4	matrix);
		void SetCameraPosition(		const string& cameraName = "camera_position",	vec3	position = vec3(0.0f));

		//////////////////////////////////////////////////////////////
		// Setting Textures
		//////////////////////////////////////////////////////////////
		void BindTexture();

		const GLuint GetProgramId() const;

		/// <summary>
		/// Checks if the uniform name is stored in cache, otherwise, adds it, and returns the location
		/// </summary>
		GLint GetUniformLocation(const string& uniformName);

	private:
		GLuint							m_shaderProgramId		= 0;
		unordered_map<string, GLint>	m_uniformLocationCache	= {}; // original does not initialiaze
	};
}