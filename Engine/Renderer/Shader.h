/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:07:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <filesystem> // Ajout pour std::filesystem
#include <glm/glm.hpp>
#include <string>
#include <unordered_set> // Ajout pour la détection de cycle d'inclusion

namespace Engine {

	class Shader {
	public:
		// Le constructeur prend toujours les chemins principaux
		Shader(const std::string &vertexPath, const std::string &fragmentPath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		// ... fonctions SetUniform* ...
		void SetUniformInt(const std::string &name, int value) const;
		void SetUniformFloat(const std::string &name, float value) const;
		void SetUniformVec3(const std::string &name, const glm::vec3 &vector) const;
		void SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const;

	private:
		unsigned int m_RendererID = 0; // Initialiser à 0

		int GetUniformLocation(const std::string &name) const;

		// Fonction principale pour charger et pré-traiter un fichier shader
		std::string LoadAndPreprocessShader(const std::filesystem::path &filepath);
		// Fonction récursive pour gérer les includes
		std::string PreprocessIncludes(const std::filesystem::path &filepath, std::unordered_set<std::string> &includedFiles);
		// Fonction simple pour lire le contenu d'un fichier
		std::string LoadFileContent(const std::string &filepath);

		unsigned int CompileShader(unsigned int type, const std::string &source);
		// CreateProgram n'est plus nécessaire si le constructeur fait tout
		// void CreateProgram(const std::string &vertexShader, const std::string &fragmentShader);
	};

} // namespace Engine