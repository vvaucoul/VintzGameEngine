/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MaterialPBR.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:38:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:15:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Texture.h"
#include <glm/glm.hpp>
#include <memory>
#include <string> // Include string header

namespace Engine {

	// Workflow Metallic-Roughness + ORM pack (R=AO, G=Roughness, B=Metallic)
	// Also allows separate AO map if ORM is not used.
	struct MaterialPBR {
		// Maps (nullptr si non fourni)
		std::shared_ptr<Texture> albedoMap;
		std::shared_ptr<Texture> normalMap;
		std::shared_ptr<Texture> aoMap; // Separate Ambient Occlusion map

		// Couleurs/facteurs par défaut si pas de map
		glm::vec3 albedoColor = {1.0f, 1.0f, 1.0f};
		float metallic		  = 1.0f; // Default if no map
		float roughness		  = 1.0f; // Default if no map
		float ao			  = 1.0f; // Default if no map

		// Flags
		bool hasAlbedoMap = false;
		bool hasNormalMap = false;
		bool hasAOMap	  = false; // Flag for separate AO map

		// Charge les textures et met à jour les flags
		void SetAlbedoMap(const std::string &path) {
			try {
				albedoMap	 = std::make_shared<Texture>(path);
				hasAlbedoMap = true;
			} catch (const std::exception &e) {
				// Handle or log error (e.g., file not found)
				hasAlbedoMap = false;
			}
		}
		void SetNormalMap(const std::string &path) {
			try {
				normalMap	 = std::make_shared<Texture>(path);
				hasNormalMap = true;
			} catch (const std::exception &e) {
				hasNormalMap = false;
			}
		}
		// Setting separate AO map
		void SetAOMap(const std::string &path) {
			try {
				aoMap	 = std::make_shared<Texture>(path);
				hasAOMap = true;
				// No longer tracking ORM here
			} catch (const std::exception &e) {
				hasAOMap = false;
			}
		}
	};

} // namespace Engine
