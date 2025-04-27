/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MaterialPBR.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:38:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:13:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Texture.h"
#include <glm/glm.hpp>
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
		std::shared_ptr<Texture> ormMap; // ORM packed: R=AO, G=Roughness, B=Metallic

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
		bool hasORMMap		 = false;

		// Setters for maps (loads texture and updates flag)
		void SetAlbedoMap(const std::string &path) {
			try {
				albedoMap	 = std::make_shared<Texture>(path);
				hasAlbedoMap = true;
			} catch (...) {
				hasAlbedoMap = false;
			}
		}
		void SetNormalMap(const std::string &path) {
			try {
				normalMap	 = std::make_shared<Texture>(path);
				hasNormalMap = true;
			} catch (...) {
				hasNormalMap = false;
			}
		}
		void SetAOMap(const std::string &path) {
			try {
				aoMap	 = std::make_shared<Texture>(path);
				hasAOMap = true;
			} catch (...) {
				hasAOMap = false;
			}
		}
		void SetRoughnessMap(const std::string &path) {
			try {
				roughnessMap	= std::make_shared<Texture>(path);
				hasRoughnessMap = true;
			} catch (...) {
				hasRoughnessMap = false;
			}
		}
		void SetMetallicMap(const std::string &path) {
			try {
				metallicMap	   = std::make_shared<Texture>(path);
				hasMetallicMap = true;
			} catch (...) {
				hasMetallicMap = false;
			}
		}
		void SetSpecularMap(const std::string &path) {
			try {
				specularMap	   = std::make_shared<Texture>(path);
				hasSpecularMap = true;
			} catch (...) {
				hasSpecularMap = false;
			}
		}
		void SetORMMap(const std::string &path) {
			try {
				ormMap	  = std::make_shared<Texture>(path);
				hasORMMap = true;
			} catch (...) {
				hasORMMap = false;
			}
		}
	};

} // namespace Engine
