/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 10:42:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace Engine {

	Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
		std::filesystem::path vp = vertexPath;
		std::filesystem::path fp = fragmentPath;

		std::string vertexSource   = LoadAndPreprocessShader(vp);
		std::string fragmentSource = LoadAndPreprocessShader(fp);

		if (vertexSource.empty() || fragmentSource.empty()) {
			std::cerr << "Error: Failed to load or preprocess shader files: " << vertexPath << ", " << fragmentPath << std::endl;
			m_IsValid = false;
			return;
		}

		unsigned int vertexShader	= CompileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
		unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

		if (vertexShader == 0 || fragmentShader == 0) {
			std::cerr << "Error: Shader compilation failed for: " << vertexPath << ", " << fragmentPath << std::endl;
			if (vertexShader) glDeleteShader(vertexShader);
			if (fragmentShader) glDeleteShader(fragmentShader);
			m_IsValid = false;
			return;
		}

		m_RendererID = glCreateProgram();
		if (!m_RendererID) {
			std::cerr << "Error: Failed to create shader program." << std::endl;
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			m_IsValid = false;
			return;
		}

		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);

		m_IsValid = LinkProgram(m_RendererID, vertexPath, fragmentPath);

		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!m_IsValid) {
			glDeleteProgram(m_RendererID);
			m_RendererID = 0;
		} else {
			std::cout << "Shader program compiled and linked: " << vertexPath << ", " << fragmentPath << " (ID: " << m_RendererID << ")" << std::endl;
		}
	}

	Shader::~Shader() {
		if (m_RendererID)
			glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const {
		if (m_IsValid)
			glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const {
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string &name, int value) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform1i(location, value);
	}

	void Shader::SetUniformFloat(const std::string &name, float value) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform1f(location, value);
	}

	void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &vector) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform3f(location, vector.x, vector.y, vector.z);
	}

	void Shader::SetUniformVec4(const std::string &name, const glm::vec4 &vector) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniformMat4(const std::string &name, const glm::mat4 &matrix) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	// Cache uniform locations for efficiency
	int Shader::GetUniformLocation(const std::string &name) {
		if (!m_IsValid) return -1;
		auto it = m_UniformLocationCache.find(name);
		if (it != m_UniformLocationCache.end())
			return it->second;
		int location				 = glGetUniformLocation(m_RendererID, name.c_str());
		m_UniformLocationCache[name] = location;
		if (location == -1)
			std::cerr << "Warning: Uniform '" << name << "' not found in shader program " << m_RendererID << std::endl;
		return location;
	}

	// Compile a shader stage (vertex or fragment), return shader ID or 0 on error
	unsigned int Shader::CompileShader(unsigned int type, const std::string &source, const std::string &originalPath) {
		if (source.empty()) {
			std::cerr << "Error: Cannot compile empty shader source for " << originalPath << std::endl;
			return 0;
		}
		unsigned int id = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> message(length);
			glGetShaderInfoLog(id, length, &length, message.data());
			std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
					  << " shader (" << originalPath << "):\n"
					  << message.data() << std::endl;
			std::cerr << "--- Shader Source (" << originalPath << ") ---\n"
					  << source << "\n--- End Source ---" << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

	// Link shaders into a program, return true if successful
	bool Shader::LinkProgram(unsigned int programID, const std::string &vp, const std::string &fp) {
		glLinkProgram(programID);
		int success;
		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[1024];
			glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Shader Linking Failed for program (" << vp << ", " << fp << "):\n"
					  << infoLog << std::endl;
			return false;
		}
		return true;
	}

	// Read file content into a string
	std::string Shader::LoadFileContent(const std::string &filepath) {
		std::ifstream file(filepath);
		if (!file.is_open()) {
			std::cerr << "Error: Could not open file: " << filepath << std::endl;
			return "";
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	// Load and preprocess shader file, handling #include recursively
	std::string Shader::LoadAndPreprocessShader(const std::filesystem::path &filepath) {
		std::unordered_set<std::string> includedFiles;
		try {
			std::filesystem::path canonicalPath = std::filesystem::canonical(filepath);
			return PreprocessIncludes(canonicalPath, includedFiles);
		} catch (const std::filesystem::filesystem_error &e) {
			std::cerr << "Filesystem error: " << e.what() << " for path: " << filepath << std::endl;
			return "";
		} catch (const std::exception &e) {
			std::cerr << "Error during shader preprocessing: " << e.what() << std::endl;
			return "";
		}
	}

	// Recursively process #include directives in GLSL files
	std::string Shader::PreprocessIncludes(const std::filesystem::path &filepath, std::unordered_set<std::string> &includedFiles) {
		std::string canonicalPathStr;
		try {
			canonicalPathStr = std::filesystem::canonical(filepath).string();
		} catch (const std::filesystem::filesystem_error &e) {
			std::cerr << "Warning: Could not get canonical path for: " << filepath << " - " << e.what() << ". Using absolute path as key." << std::endl;
			canonicalPathStr = std::filesystem::absolute(filepath).string();
		}

		// Prevent duplicate includes
		if (includedFiles.count(canonicalPathStr))
			return "";
		includedFiles.insert(canonicalPathStr);

		std::string source = LoadFileContent(filepath.string());
		if (source.empty()) {
			includedFiles.erase(canonicalPathStr);
			return "";
		}

		std::filesystem::path currentDir = filepath.parent_path();
		std::stringstream outputSource;
		std::stringstream inputSource(source);
		std::string line;
		std::regex includeRegex(R"(\s*#\s*include\s*["<]([^">]+)[">].*)");
		size_t lineNumber			 = 0;
		std::string directivePathStr = !canonicalPathStr.empty() ? canonicalPathStr : filepath.string();

		while (std::getline(inputSource, line)) {
			lineNumber++;
			std::smatch match;
			if (std::regex_match(line, match, includeRegex)) {
				if (match.size() == 2) {
					std::string includeFilename		  = match[1].str();
					std::filesystem::path includePath = currentDir / includeFilename;
					includePath						  = includePath.lexically_normal();

					try {
						std::filesystem::path canonicalIncludePath = std::filesystem::canonical(includePath);
						std::string includedContent				   = PreprocessIncludes(canonicalIncludePath, includedFiles);

						if (!includedContent.empty() && includedContent.back() != '\n')
							includedContent += '\n';

						outputSource << includedContent;
					} catch (const std::filesystem::filesystem_error &e) {
						std::cerr << "Error resolving include path: " << e.what() << " for include: \"" << includeFilename << "\" (resolved to: " << includePath << ") in file: " << directivePathStr << std::endl;
						outputSource << "// Error including file: " << includeFilename << "\n";
					}
				} else {
					outputSource << line << "\n";
				}
			} else {
				outputSource << line << "\n";
			}
		}
		return outputSource.str();
	}

} // namespace Engine
