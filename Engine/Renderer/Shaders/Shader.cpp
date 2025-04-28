/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 17:04:15 by vvaucoul         ###   ########.fr       */
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

	// --- Constructor ---
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

		// Check compilation before linking
		if (vertexShader == 0 || fragmentShader == 0) {
			std::cerr << "Error: Shader compilation failed. Cannot create program for " << vertexPath << ", " << fragmentPath << std::endl;
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			m_IsValid = false;
			return;
		}

		m_RendererID = glCreateProgram();
		if (m_RendererID == 0) {
			std::cerr << "Error: Failed to create shader program." << std::endl;
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			m_IsValid = false;
			return;
		}

		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);

		// Link the program
		m_IsValid = LinkProgram(m_RendererID, vertexShader, fragmentShader, vertexPath, fragmentPath);

		// Detach and delete shaders after linking (whether successful or not)
		// They are no longer needed after being linked into the program object.
		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!m_IsValid) {
			glDeleteProgram(m_RendererID); // Clean up failed program
			m_RendererID = 0;
		} else {
			std::cout << "Successfully compiled and linked shader program: " << vertexPath << ", " << fragmentPath << " (ID: " << m_RendererID << ")" << std::endl;
		}
	}

	// --- Destructor ---
	Shader::~Shader() {
		if (m_RendererID != 0) {
			glDeleteProgram(m_RendererID);
		}
	}

	// --- Public Methods ---
	void Shader::Bind() const {
		// Only bind if the shader is valid
		if (m_IsValid) {
			glUseProgram(m_RendererID);
		} else {
			// Optionally print a warning if trying to bind an invalid shader
			// std::cerr << "Warning: Attempting to bind an invalid shader program." << std::endl;
		}
	}

	void Shader::Unbind() const {
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string &name, int value) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1) {
			glUniform1i(location, value);
		}
	}

	void Shader::SetUniformFloat(const std::string &name, float value) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1) {
			glUniform1f(location, value);
		}
	}

	void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &vector) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1) {
			glUniform3f(location, vector.x, vector.y, vector.z);
		}
	}

	void Shader::SetUniformVec4(const std::string &name, const glm::vec4 &vector) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1) {
			glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
		}
	}

	void Shader::SetUniformMat4(const std::string &name, const glm::mat4 &matrix) {
		if (!m_IsValid) return;
		int location = GetUniformLocation(name);
		if (location != -1) {
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)); // Use glm::value_ptr
		}
	}

	// --- Private Helper Methods ---

	// Gets uniform location, using cache
	int Shader::GetUniformLocation(const std::string &name) {
		if (!m_IsValid) return -1; // Don't query uniforms on invalid shaders

		// Check cache first
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
			return m_UniformLocationCache[name];
		}

		// Not in cache, query OpenGL
		int location = glGetUniformLocation(m_RendererID, name.c_str());

		// Store in cache (even if -1, to avoid repeated failed lookups)
		m_UniformLocationCache[name] = location;

		if (location == -1) {
			// Warning: This can be noisy if uniforms are optimized out.
			// Only enable for debugging specific uniform issues.
			std::cerr << "Warning: Uniform '" << name << "' not found or not active in shader program ID " << m_RendererID << "." << std::endl;
		}
		return location;
	}

	// Compiles a shader stage, returns shader ID or 0 on failure
	unsigned int Shader::CompileShader(unsigned int type, const std::string &source, const std::string &originalPath) {
		if (source.empty()) {
			std::cerr << "Error: Cannot compile empty shader source for " << originalPath << "." << std::endl;
			return 0;
		}

		unsigned int id = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		// Error checking
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> message(length);
			glGetShaderInfoLog(id, length, &length, message.data());
			std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
					  << " shader (" << originalPath << ")!" << std::endl;
			std::cerr << message.data() << std::endl;
			// Optional: Print preprocessed source on error
			// std::cerr << "--- Shader Source (" << originalPath << ") ---" << std::endl << source << std::endl << "--- End Source ---" << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

	// Links the shader program, returns true on success, false on failure
	bool Shader::LinkProgram(unsigned int programID, unsigned int vertexShader, unsigned int fragmentShader, const std::string &vp, const std::string &fp) {
		glLinkProgram(programID);

		// Check linking status
		int success;
		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[1024];
			glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Shader Linking Failed for program using (" << vp << ", " << fp << "):\n"
					  << infoLog << std::endl;
			return false;
		}

		// Optional: Validate program (checks if it can execute given current GL state)
		// Can be slow, use mainly for debugging.
		// glValidateProgram(programID);
		// glGetProgramiv(programID, GL_VALIDATE_STATUS, &success);
		// if (!success) {
		//     char infoLog[1024];
		//     glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
		//     std::cerr << "Shader Validation Failed for program using (" << vp << ", " << fp << "):\n"
		//               << infoLog << std::endl;
		//     // return false; // Decide if validation failure should prevent use
		// }

		return true;
	}

	// --- Preprocessing Logic (No changes needed from your version) ---
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

	std::string Shader::PreprocessIncludes(const std::filesystem::path &filepath, std::unordered_set<std::string> &includedFiles) {
		std::string canonicalPathStr = filepath.string();

		if (includedFiles.count(canonicalPathStr)) {
			return ""; // Already included
		}
		includedFiles.insert(canonicalPathStr);

		std::string source = LoadFileContent(canonicalPathStr);
		if (source.empty()) {
			includedFiles.erase(canonicalPathStr);
			return "";
		}

		std::filesystem::path currentDir = filepath.parent_path();
		std::stringstream outputSource;
		std::stringstream inputSource(source);
		std::string line;
		std::regex includeRegex(R"(\s*#\s*include\s*["<]([^">]+)[">])");
		size_t lineNumber = 0;

		while (std::getline(inputSource, line)) {
			lineNumber++;
			std::smatch match;
			if (std::regex_match(line, match, includeRegex)) {
				if (match.size() == 2) {
					std::string includeFilename		  = match[1].str();
					std::filesystem::path includePath = currentDir / includeFilename;
					try {
						std::filesystem::path canonicalIncludePath = std::filesystem::canonical(includePath);
						std::string includedContent				   = PreprocessIncludes(canonicalIncludePath, includedFiles); // Recursive call

						// Add #line directives for better debugging
						outputSource << "#line " << 1 << " \"" << canonicalIncludePath.string() << "\"\n";
						outputSource << includedContent << "\n";
						outputSource << "#line " << (lineNumber + 1) << " \"" << canonicalPathStr << "\"\n";

					} catch (const std::filesystem::filesystem_error &e) {
						std::cerr << "Error resolving include path: " << e.what() << " for include: \"" << includeFilename << "\" in file: " << canonicalPathStr << std::endl;
						outputSource << "// Error including file: " << includeFilename << "\n";
					}
				} else {
					outputSource << line << "\n";
				}
			} else {
				outputSource << line << "\n";
			}
		}

		includedFiles.erase(canonicalPathStr); // Allows re-inclusion via different paths if needed
		return outputSource.str();
	}

} // namespace Engine