/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:43:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Engine {

	class Shader {
	public:
		Shader(const std::string &vertexPath, const std::string &fragmentPath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformInt(const std::string &name, int value) const;
		void SetUniformFloat(const std::string &name, float value) const;
		void SetUniformVec3(const std::string &name, const glm::vec3 &vector) const;
		void SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const;

	private:
		unsigned int m_RendererID;

		int GetUniformLocation(const std::string &name) const;
		std::string LoadFile(const std::string &filepath);
		unsigned int CompileShader(unsigned int type, const std::string &source);
		void CreateProgram(const std::string &vertexShader, const std::string &fragmentShader);
	};

} // namespace Engine
