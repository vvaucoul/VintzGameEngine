/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 16:50:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Engine {

	class Shader {
	public:
		Shader(const std::string &vertexPath, const std::string &fragmentPath);
		~Shader();

		// Check if the shader compiled and linked successfully
		bool IsValid() const { return m_IsValid; }

		void Bind() const;
		void Unbind() const;

		// Uniform setters (now use cached locations)
		void SetUniformInt(const std::string &name, int value);
		void SetUniformFloat(const std::string &name, float value);
		void SetUniformVec3(const std::string &name, const glm::vec3 &vector);
		void SetUniformVec4(const std::string &name, const glm::vec4 &vector);
		void SetUniformMat4(const std::string &name, const glm::mat4 &matrix);

		// Getter for the OpenGL program ID (optional, for advanced use)
		unsigned int GetRendererID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID = 0;
		bool m_IsValid			  = false; // Flag to indicate successful compilation and linking
		// Cache for uniform locations
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;

		// --- Private Helper Methods ---
		int GetUniformLocation(const std::string &name); // Changed return type, now uses cache

		std::string LoadAndPreprocessShader(const std::filesystem::path &filepath);
		std::string PreprocessIncludes(const std::filesystem::path &filepath, std::unordered_set<std::string> &includedFiles);
		std::string LoadFileContent(const std::string &filepath);

		unsigned int CompileShader(unsigned int type, const std::string &source, const std::string &originalPath); // Pass path for error messages
		bool LinkProgram(unsigned int programID, unsigned int vertexShader, unsigned int fragmentShader, const std::string &vp, const std::string &fp);
	};

} // namespace Engine