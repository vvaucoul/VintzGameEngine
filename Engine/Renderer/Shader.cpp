/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:50:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Shader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> // For error logging
#include <sstream>

namespace Engine {

	Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
		std::string vertexSource   = LoadFile(vertexPath);
		std::string fragmentSource = LoadFile(fragmentPath);

		unsigned int vertexShader	= CompileShader(GL_VERTEX_SHADER, vertexSource);
		unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		int success;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
			std::cerr << "Shader Linking Failed:\n"
					  << infoLog << std::endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader() {
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const {
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const {
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string &name, int value) const {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniformFloat(const std::string &name, float value) const {
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &vector) const {
		glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const {
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}

	int Shader::GetUniformLocation(const std::string &name) const {
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
			std::cerr << "Warning: uniform '" << name << "' doesn't exist or is not used in shader!" << std::endl;
		return location;
	}

	std::string Shader::LoadFile(const std::string &filepath) {
		std::ifstream file(filepath);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string &source) {
		unsigned int id = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		// Error handling
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char *message = (char *)alloca(length * sizeof(char)); // Allocate on stack
			glGetShaderInfoLog(id, length, &length, message);
			std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
			std::cerr << message << std::endl;
			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	void Shader::CreateProgram(const std::string &vertexShader, const std::string &fragmentShader) {
		m_RendererID	= glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		// Check if compilation succeeded before attaching
		if (vs == 0 || fs == 0) {
			glDeleteProgram(m_RendererID);
			m_RendererID = 0; // Indicate failure
			// Clean up potentially created shader objects
			if (vs != 0)
				glDeleteShader(vs);
			if (fs != 0)
				glDeleteShader(fs);
			return; // Stop if compilation failed
		}

		glAttachShader(m_RendererID, vs);
		glAttachShader(m_RendererID, fs);
		glLinkProgram(m_RendererID);

		// Error handling for linking
		int result;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
			char *message = (char *)alloca(length * sizeof(char));
			glGetProgramInfoLog(m_RendererID, length, &length, message);
			std::cerr << "Failed to link shader program!" << std::endl;
			std::cerr << message << std::endl;
			glDeleteProgram(m_RendererID);
			m_RendererID = 0; // Indicate failure
							  // Shaders are already attached, they will be deleted when the program is deleted if linking fails,
							  // but we still need to detach and delete if linking succeeds.
		} else {
			glValidateProgram(m_RendererID); // Optional: Check if the program can execute
		}

		// Detach and delete shaders after successful linking
		// If linking failed, the program is already deleted or marked invalid.
		if (m_RendererID != 0) {
			glDetachShader(m_RendererID, vs);
			glDetachShader(m_RendererID, fs);
		}
		// Delete shaders regardless of linking success, as they are now part of the program or failed compilation
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

} // namespace Engine
