#pragma once

#include <string>
#include "Error.hpp"
#include <iostream>
#include <unordered_map>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"


namespace Vicetrice
{
	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader
	{
	public:

		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4f(const std::string& name, const float(&v)[4]);
		void SetUniform1f(const std::string& name, float v0);
		void SetUniform1i(const std::string& name, int v0);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	private:
		unsigned int m_RendererID;
		std::unordered_map<std::string, int> m_UlocationCache;



		int GetUniformLocation(const std::string& name);

		struct ShaderProgramSource ParseShader(const std::string& filepath);

		unsigned int CompileShader(unsigned int type, const std::string& source);

		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	}; //class Shader
} //namespace Vicetrice
