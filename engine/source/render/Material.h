#pragma once
#include <unordered_map>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

using std::unordered_map;
using std::string;

namespace eng
{
	class ShaderProgram;

	class Material
	{
	public:
		//Material() = delete;
		~Material();
		// original parameter: const std::shared_ptr<ShaderProgram>& shaderProgram
		void SetShaderProgram(ShaderProgram* shaderProgram);
		[[nodiscard]] ShaderProgram* GetShaderProgram() const;

		void SetMaterialParameters(const string& paramName, float value);
		void SetMaterialProjectionMatrix(const string& matrixName, const mat4& matrix) const;
		void SetCameraPosition(const string& cameraName, const vec3& position) const;

		void Bind();

	private:
		// original std::shared_ptr<ShaderProgram>
		ShaderProgram*					m_shaderProgram		= nullptr;
		unordered_map<string, float>	m_parameters		= {};

	};
}