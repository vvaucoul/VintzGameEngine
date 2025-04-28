/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:43:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 14:05:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Shader.h"

#include <filesystem> // Nécessaire
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <regex> // Nécessaire pour trouver les #include
#include <sstream>
#include <unordered_set> // Nécessaire
#include <vector>		 // Pour les messages d'erreur de compilation

namespace Engine {

	// Constructeur modifié pour utiliser le pré-processeur
	Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
		std::filesystem::path vp = vertexPath;
		std::filesystem::path fp = fragmentPath;

		std::string vertexSource   = LoadAndPreprocessShader(vp);
		std::string fragmentSource = LoadAndPreprocessShader(fp);

		if (vertexSource.empty() || fragmentSource.empty()) {
			std::cerr << "Error: Failed to load or preprocess shader files." << std::endl;
			// m_RendererID est déjà 0
			return;
		}

		unsigned int vertexShader	= CompileShader(GL_VERTEX_SHADER, vertexSource);
		unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

		// Vérifier la compilation avant de lier
		if (vertexShader == 0 || fragmentShader == 0) {
			std::cerr << "Error: Shader compilation failed. Cannot link program." << std::endl;
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			// m_RendererID est déjà 0
			return;
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		// Vérifier le lien
		int success;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[1024]; // Augmenter la taille si nécessaire
			glGetProgramInfoLog(m_RendererID, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Shader Linking Failed for " << vertexPath << " and " << fragmentPath << ":\n"
					  << infoLog << std::endl;
			glDeleteProgram(m_RendererID); // Nettoyer le programme échoué
			m_RendererID = 0;			   // Indiquer l'échec
		} else {
			glValidateProgram(m_RendererID); // Validation optionnelle
			// Détacher les shaders après un lien réussi (bonne pratique)
			glDetachShader(m_RendererID, vertexShader);
			glDetachShader(m_RendererID, fragmentShader);
		}

		// Supprimer les shaders car ils sont maintenant liés (ou ont échoué)
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader() {
		if (m_RendererID != 0) { // Ne supprimer que si la création a réussi
			glDeleteProgram(m_RendererID);
		}
	}

	// --- Logique de pré-traitement ---

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
			// Utiliser les chemins canoniques pour une meilleure gestion des doublons/liens symboliques
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

		// Empêcher l'inclusion récursive directe
		if (includedFiles.count(canonicalPathStr)) {
			return ""; // Déjà inclus dans cette chaîne d'inclusion
		}
		includedFiles.insert(canonicalPathStr);

		std::string source = LoadFileContent(canonicalPathStr);
		if (source.empty()) {
			includedFiles.erase(canonicalPathStr); // Retirer si le chargement échoue
			return "";							   // Erreur déjà affichée
		}

		std::filesystem::path currentDir = filepath.parent_path();
		std::stringstream outputSource;
		std::stringstream inputSource(source);
		std::string line;
		// Regex simple pour #include "fichier" ou #include <fichier>
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
						// Résoudre le chemin canonique pour la vérification d'inclusion
						std::filesystem::path canonicalIncludePath = std::filesystem::canonical(includePath);
						std::string includedContent				   = PreprocessIncludes(canonicalIncludePath, includedFiles); // Appel récursif

						// Ajouter des directives #line pour conserver les informations de débogage
						outputSource << "#line " << 1 << " \"" << canonicalIncludePath.string() << "\"\n";
						outputSource << includedContent << "\n";
						outputSource << "#line " << (lineNumber + 1) << " \"" << canonicalPathStr << "\"\n";

					} catch (const std::filesystem::filesystem_error &e) {
						std::cerr << "Error resolving include path: " << e.what() << " for include: \"" << includeFilename << "\" in file: " << canonicalPathStr << std::endl;
						outputSource << "// Error including file: " << includeFilename << "\n";
					}
				} else {
					// Directive include mal formée?
					outputSource << line << "\n";
				}
			} else {
				outputSource << line << "\n";
			}
		}

		// Retirer le fichier de l'ensemble *après* l'avoir traité.
		// Cela permet à un même fichier d'être inclus plusieurs fois via des chemins différents,
		// mais empêche une boucle directe A -> B -> A.
		// Si vous voulez un comportement strict #pragma once / #ifndef, ne retirez pas le fichier.
		includedFiles.erase(canonicalPathStr);

		return outputSource.str();
	}

	// --- Autres méthodes de Shader ---

	void Shader::Bind() const {
		if (m_RendererID != 0) glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const {
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string &name, int value) const {
		if (m_RendererID != 0) glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniformFloat(const std::string &name, float value) const {
		if (m_RendererID != 0) glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &vector) const {
		if (m_RendererID != 0) glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniformVec4(const std::string &name, const glm::vec4 &vector) const {
		if (m_RendererID != 0) glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniformMat4(const std::string &name, const glm::mat4 &matrix) const {
		if (m_RendererID != 0) glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}

	int Shader::GetUniformLocation(const std::string &name) const {
		if (m_RendererID == 0) return -1;
		// TODO: Cache uniform locations for performance
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1) {
			// Ne pas spammer la console pour les uniforms optionnels (ex: textures non utilisées)
			// std::cerr << "Warning: uniform '" << name << "' doesn't exist or is not used!" << std::endl;
		}
		return location;
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string &source) {
		if (source.empty()) {
			std::cerr << "Error: Cannot compile empty shader source for type " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "." << std::endl;
			return 0;
		}
		unsigned int id = glCreateShader(type);
		const char *src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		// Gestion des erreurs de compilation
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> message(length); // Utiliser un vecteur pour taille dynamique
			glGetShaderInfoLog(id, length, &length, message.data());
			std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
			std::cerr << message.data() << std::endl;
			// Optionnel: Afficher la source pré-traitée qui a échoué
			std::cerr << "--- Shader Source ---" << std::endl
					  << source << std::endl
					  << "--- End Source ---" << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

} // namespace Engine