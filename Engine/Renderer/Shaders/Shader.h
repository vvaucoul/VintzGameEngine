/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 10:29:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Engine {

	/**
	 * @brief OpenGL Shader abstraction supporting #include preprocessing and uniform caching.
	 *
	 * Loads, compiles, and links vertex/fragment shaders from file. Supports recursive #include
	 * directives in GLSL source. Uniform locations are cached for efficient updates.
	 */
	class Shader {
	public:
		/**
		 * @brief Construct and compile a shader program from vertex and fragment source files.
		 * @param vertexPath   Path to the vertex shader file.
		 * @param fragmentPath Path to the fragment shader file.
		 */
		Shader(const std::string &vertexPath, const std::string &fragmentPath);

		/**
		 * @brief Destructor. Deletes the OpenGL shader program.
		 */
		~Shader();

		/**
		 * @brief Check if the shader compiled and linked successfully.
		 * @return True if valid, false otherwise.
		 */
		bool IsValid() const { return m_IsValid; }

		/**
		 * @brief Bind (activate) this shader program for rendering.
		 */
		void Bind() const;

		/**
		 * @brief Unbind (deactivate) any shader program.
		 */
		void Unbind() const;

		// --- Uniform Setters (with location caching) ---

		/**
		 * @brief Set an integer uniform.
		 * @param name  Uniform variable name.
		 * @param value Integer value.
		 */
		void SetUniformInt(const std::string &name, int value);

		/**
		 * @brief Set a float uniform.
		 * @param name  Uniform variable name.
		 * @param value Float value.
		 */
		void SetUniformFloat(const std::string &name, float value);

		/**
		 * @brief Set a vec3 uniform.
		 * @param name   Uniform variable name.
		 * @param vector glm::vec3 value.
		 */
		void SetUniformVec3(const std::string &name, const glm::vec3 &vector);

		/**
		 * @brief Set a vec4 uniform.
		 * @param name   Uniform variable name.
		 * @param vector glm::vec4 value.
		 */
		void SetUniformVec4(const std::string &name, const glm::vec4 &vector);

		/**
		 * @brief Set a mat4 uniform.
		 * @param name   Uniform variable name.
		 * @param matrix glm::mat4 value.
		 */
		void SetUniformMat4(const std::string &name, const glm::mat4 &matrix);

		/**
		 * @brief Get the OpenGL program ID (advanced use).
		 * @return GLuint program ID.
		 */
		unsigned int GetRendererID() const { return m_RendererID; }

	private:
		unsigned int m_RendererID = 0;	   ///< OpenGL program object ID.
		bool m_IsValid			  = false; ///< Compilation/link status.

		// Uniform location cache for fast lookup.
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;

		/**
		 * @brief Get (and cache) the location of a uniform variable.
		 * @param name Uniform variable name.
		 * @return Location or -1 if not found.
		 */
		int GetUniformLocation(const std::string &name);

		/**
		 * @brief Load and preprocess a shader file (handles #include recursively).
		 * @param filepath Path to the shader file.
		 * @return Preprocessed GLSL source as string.
		 */
		std::string LoadAndPreprocessShader(const std::filesystem::path &filepath);

		/**
		 * @brief Recursively process #include directives in a shader file.
		 * @param filepath      Path to the current file.
		 * @param includedFiles Set of already-included files (to prevent cycles).
		 * @return Preprocessed GLSL source.
		 */
		std::string PreprocessIncludes(const std::filesystem::path &filepath, std::unordered_set<std::string> &includedFiles);

		/**
		 * @brief Load the entire contents of a file into a string.
		 * @param filepath Path to the file.
		 * @return File contents as string, or empty string on error.
		 */
		std::string LoadFileContent(const std::string &filepath);

		/**
		 * @brief Compile a GLSL shader from source.
		 * @param type         GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
		 * @param source       GLSL source code.
		 * @param originalPath Path for error reporting.
		 * @return Shader object ID, or 0 on failure.
		 */
		unsigned int CompileShader(unsigned int type, const std::string &source, const std::string &originalPath);

		/**
		 * @brief Link vertex and fragment shaders into a program.
		 * @param programID    OpenGL program object.
		 * @param vp           Vertex shader path (for error reporting).
		 * @param fp           Fragment shader path (for error reporting).
		 * @return True if linking succeeded, false otherwise.
		 */
		bool LinkProgram(unsigned int programID, const std::string &vp, const std::string &fp);
	};

} // namespace Engine