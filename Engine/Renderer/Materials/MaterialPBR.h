/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MaterialPBR.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:38:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:50:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Renderer/Textures/Texture.h" // Includes ResamplingAlgorithm enum
#include <glm/glm.hpp>
#include <iostream> // For error logging
#include <memory>
#include <string>

namespace Engine {

	/**
	 * @struct MaterialPBR
	 * @brief Physically-Based Rendering material supporting multiple texture workflows.
	 *
	 * Supports separate or packed (ORM) maps for albedo, normal, roughness, metallic, specular, and AO.
	 * If a map is not provided, the corresponding scalar factor is used.
	 */
	struct MaterialPBR {
		// Texture maps (nullptr if not provided)
		std::shared_ptr<Texture> albedoMap;
		std::shared_ptr<Texture> normalMap;
		std::shared_ptr<Texture> aoMap; // Ambient Occlusion
		std::shared_ptr<Texture> roughnessMap;
		std::shared_ptr<Texture> metallicMap;
		std::shared_ptr<Texture> specularMap;

		// Default factors/colors if no map
		glm::vec3 albedoColor = {1.0f, 1.0f, 1.0f};
		float metallic		  = 1.0f;
		float roughness		  = 1.0f;
		float ao			  = 1.0f;
		float specular		  = 0.5f;

		// Flags
		bool hasAlbedoMap	 = false;
		bool hasNormalMap	 = false;
		bool hasAOMap		 = false;
		bool hasRoughnessMap = false;
		bool hasMetallicMap	 = false;
		bool hasSpecularMap	 = false;

		// Setters for maps (loads texture with optional resizing and updates flag)
		void SetAlbedoMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				albedoMap	 = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasAlbedoMap = (albedoMap && albedoMap->GetID() != 0); // Check if texture loaded successfully
			} catch (const std::exception &e) {
				std::cerr << "Error loading Albedo map '" << path << "': " << e.what() << std::endl;
				hasAlbedoMap = false;
				albedoMap	 = nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading Albedo map '" << path << "'" << std::endl;
				hasAlbedoMap = false;
				albedoMap	 = nullptr;
			}
		}
		void SetNormalMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				normalMap	 = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasNormalMap = (normalMap && normalMap->GetID() != 0);
			} catch (const std::exception &e) {
				std::cerr << "Error loading Normal map '" << path << "': " << e.what() << std::endl;
				hasNormalMap = false;
				normalMap	 = nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading Normal map '" << path << "'" << std::endl;
				hasNormalMap = false;
				normalMap	 = nullptr;
			}
		}
		void SetAOMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				aoMap	 = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasAOMap = (aoMap && aoMap->GetID() != 0);
			} catch (const std::exception &e) {
				std::cerr << "Error loading AO map '" << path << "': " << e.what() << std::endl;
				hasAOMap = false;
				aoMap	 = nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading AO map '" << path << "'" << std::endl;
				hasAOMap = false;
				aoMap	 = nullptr;
			}
		}
		void SetRoughnessMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				roughnessMap	= std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasRoughnessMap = (roughnessMap && roughnessMap->GetID() != 0);
			} catch (const std::exception &e) {
				std::cerr << "Error loading Roughness map '" << path << "': " << e.what() << std::endl;
				hasRoughnessMap = false;
				roughnessMap	= nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading Roughness map '" << path << "'" << std::endl;
				hasRoughnessMap = false;
				roughnessMap	= nullptr;
			}
		}
		void SetMetallicMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				metallicMap	   = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasMetallicMap = (metallicMap && metallicMap->GetID() != 0);
			} catch (const std::exception &e) {
				std::cerr << "Error loading Metallic map '" << path << "': " << e.what() << std::endl;
				hasMetallicMap = false;
				metallicMap	   = nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading Metallic map '" << path << "'" << std::endl;
				hasMetallicMap = false;
				metallicMap	   = nullptr;
			}
		}
		void SetSpecularMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			try {
				specularMap	   = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasSpecularMap = (specularMap && specularMap->GetID() != 0);
			} catch (const std::exception &e) {
				std::cerr << "Error loading Specular map '" << path << "': " << e.what() << std::endl;
				hasSpecularMap = false;
				specularMap	   = nullptr;
			} catch (...) {
				std::cerr << "Unknown error loading Specular map '" << path << "'" << std::endl;
				hasSpecularMap = false;
				specularMap	   = nullptr;
			}
		}
	};

} // namespace Engine
