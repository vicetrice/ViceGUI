#include "Shader.hpp"
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include "Error.hpp"
#include <iostream>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

namespace Vicetrice
{
	Shader::Shader(const std::string& filepath, unsigned int* CurrentID) : m_RendererID{ 0 }, m_CurrentID{ CurrentID }
	{
		ShaderProgramSource source = ParseShader(filepath);

		std::cout << "VERTEX" << std::endl << source.VertexSource << std::endl;
		std::cout << "FRAGMENT" << std::endl << source.FragmentSource << std::endl;

		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	Shader::~Shader()
	{
		GLCall(glDeleteProgram(m_RendererID));

	}

	void Shader::Bind() const
	{
		if (*m_CurrentID != m_RendererID)
		{
			GLCall(glUseProgram(m_RendererID));
			*m_CurrentID = m_RendererID;
		}
	}

	void Shader::Unbind() const
	{
		GLCall(glUseProgram(0));
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));

	}

	void Shader::SetUniform4f(const std::string& name, const float(&v)[4])
	{
		GLCall(glUniform4f(GetUniformLocation(name), v[0], v[1], v[2], v[3]));
	}

	void Shader::SetUniform1f(const std::string& name, float v0)
	{
		GLCall(glUniform1f(GetUniformLocation(name), v0));
	}

	void Shader::SetUniform1i(const std::string& name, int v0)
	{
		GLCall(glUniform1i(GetUniformLocation(name), v0));
	}

	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	}


	int Shader::GetUniformLocation(const std::string& name)
	{
		if (m_UlocationCache.find(name) != m_UlocationCache.end())
		{
			return m_UlocationCache.at(name);
		}
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		assert(location != -1);

		m_UlocationCache[name] = location;
		return location;
	}

	ShaderProgramSource Shader::ParseShader(const std::string& filepath)
	{
		enum class ShaderType
		{
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::ifstream stream(filepath);
		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;

		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			}
			else
			{
				ss[(int)type] << line << '\n';
			}
		}

		return { ss[0].str(), ss[1].str() };

	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
	{
		GLCall(unsigned int id = glCreateShader(type));
		const char* src = source.c_str();
		GLCall(glShaderSource(id, 1, &src, nullptr));
		GLCall(glCompileShader(id));

		// Error handling
		int result;
		GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;
		if (result == GL_FALSE)
		{
			int length;
			GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
			char* message = (char*)_malloca(length * sizeof(char));
			GLCall(glGetShaderInfoLog(id, length, &length, message));
			std::cout
				<< "Failed to compile "
				<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
				<< "shader"
				<< std::endl;
			std::cout << message << std::endl;
			GLCall(glDeleteShader(id));
			return 0;
		}

		return id;
	}

	unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		// create a shader program
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		GLCall(glAttachShader(program, vs));
		GLCall(glAttachShader(program, fs));

		GLCall(glLinkProgram(program));

		GLint program_linked;

		GLCall(glGetProgramiv(program, GL_LINK_STATUS, &program_linked));
		std::cout << "Program link status: " << program_linked << std::endl;
		if (program_linked != GL_TRUE)
		{
			GLsizei log_length = 0;
			GLchar message[1024];
			GLCall(glGetProgramInfoLog(program, 1024, &log_length, message));
			std::cout << "Failed to link program" << std::endl;
			std::cout << message << std::endl;
		}

		GLCall(glValidateProgram(program));

		GLCall(glDeleteShader(vs));
		GLCall(glDeleteShader(fs));

		return program;
	}
} //namespace Vicetrice