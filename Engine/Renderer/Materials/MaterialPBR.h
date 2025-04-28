/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MaterialPBR.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:38:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 17:46:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderer/Textures/Texture.h" // For Texture and ResamplingAlgorithm
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace Engine {

	/**
	 * @struct MaterialPBR
	 * @brief Represents a physically-based rendering (PBR) material with support for multiple texture workflows.
	 *
	 * This structure encapsulates all properties required for PBR shading, including:
	 *   - Albedo (diffuse), normal, roughness, metallic, specular, and ambient occlusion (AO) maps.
	 *   - Scalar fallback values for each property if a texture map is not provided.
	 *   - Boolean flags indicating the presence of each map.
	 *
	 * Texture maps are loaded via the provided setters, which handle error checking and flag updates.
	 */
	struct MaterialPBR {
		// --- Texture Maps (nullptr if not assigned) ---
		std::shared_ptr<Texture> albedoMap;		///< Albedo (diffuse) texture map
		std::shared_ptr<Texture> normalMap;		///< Normal map
		std::shared_ptr<Texture> aoMap;			///< Ambient occlusion map
		std::shared_ptr<Texture> roughnessMap;	///< Roughness map
		std::shared_ptr<Texture> metallicMap;	///< Metallic map
		std::shared_ptr<Texture> specularMap;	///< Specular map
		std::shared_ptr<Texture> emissiveMap;	///< Emissive color map
		std::shared_ptr<Texture> opacityMap;	///< Opacity map (alpha channel often used)
		std::shared_ptr<Texture> heightMap;		///< Height map for parallax/displacement
		std::shared_ptr<Texture> clearcoatMap;	///< Clearcoat intensity/roughness map (can pack values)
		std::shared_ptr<Texture> anisotropyMap; ///< Anisotropy direction/strength map
		std::shared_ptr<Texture> subsurfaceMap; ///< Subsurface scattering thickness/color map

		// --- Scalar Factors (used if corresponding map is missing) ---
		glm::vec3 albedoColor{1.0f, 1.0f, 1.0f};		 ///< Default albedo color (white)
		float metallic	= 1.0f;							 ///< Default metallic factor
		float roughness = 1.0f;							 ///< Default roughness factor
		float ao		= 1.0f;							 ///< Default ambient occlusion factor
		float specular	= 0.5f;							 ///< Default specular factor
		glm::vec3 emissiveColor{0.0f, 0.0f, 0.0f};		 ///< Default emissive color (black)
		float opacity			 = 1.0f;				 ///< Default opacity (fully opaque)
		float parallaxScale		 = 0.05f;				 ///< Default scale for parallax effect
		float clearcoat			 = 0.0f;				 ///< Default clearcoat intensity (none)
		float clearcoatRoughness = 0.1f;				 ///< Default clearcoat roughness
		glm::vec3 sheenColor{0.0f, 0.0f, 0.0f};			 ///< Default sheen color (none)
		float sheenRoughness = 0.3f;					 ///< Default sheen roughness
		float anisotropy	 = 0.0f;					 ///< Default anisotropy strength (isotropic)
		glm::vec3 anisotropyDirection{1.0f, 0.0f, 0.0f}; ///< Default anisotropy direction (tangent)
		float subsurface = 0.0f;						 ///< Default SSS amount (none)
		glm::vec3 subsurfaceColor{1.0f, 1.0f, 1.0f};	 ///< Default SSS color tint (white)

		// --- Map Presence Flags ---
		bool hasAlbedoMap	  = false; ///< True if albedoMap is valid
		bool hasNormalMap	  = false; ///< True if normalMap is valid
		bool hasAOMap		  = false; ///< True if aoMap is valid
		bool hasRoughnessMap  = false; ///< True if roughnessMap is valid
		bool hasMetallicMap	  = false; ///< True if metallicMap is valid
		bool hasSpecularMap	  = false; ///< True if specularMap is valid
		bool hasEmissiveMap	  = false; ///< True if emissiveMap is valid
		bool hasOpacityMap	  = false; ///< True if opacityMap is valid
		bool hasHeightMap	  = false; ///< True if heightMap is valid
		bool hasClearcoatMap  = false; ///< True if clearcoatMap is valid
		bool hasAnisotropyMap = false; ///< True if anisotropyMap is valid
		bool hasSubsurfaceMap = false; ///< True if subsurfaceMap is valid

		// --- Other Properties ---
		bool doubleSided = false; ///< Render both front and back faces?

		/**
		 * @brief Helper to load a texture map and update its presence flag.
		 *
		 * @param[out] texturePtr   Reference to the shared_ptr<Texture> to assign.
		 * @param[out] hasMapFlag   Reference to the boolean flag indicating map presence.
		 * @param[in]  mapType      Human-readable map type (for logging).
		 * @param[in]  path         Filesystem path to the texture.
		 * @param[in]  targetWidth  Desired width (0 = original).
		 * @param[in]  targetHeight Desired height (0 = original).
		 * @param[in]  algorithm    Resampling algorithm for resizing.
		 */
		void LoadTextureMap(std::shared_ptr<Texture> &texturePtr, bool &hasMapFlag, const std::string &mapType, const std::string &path, int targetWidth, int targetHeight, ResamplingAlgorithm algorithm) {
			try {
				texturePtr = std::make_shared<Texture>(path, targetWidth, targetHeight, algorithm);
				hasMapFlag = (texturePtr && texturePtr->GetID() != 0);
				if (!hasMapFlag) {
					std::cerr << "[MaterialPBR] Warning: Loaded " << mapType << " map from '" << path
							  << "' but texture is invalid (ID=0)." << std::endl;
				}
			} catch (const std::exception &e) {
				std::cerr << "[MaterialPBR] Error loading " << mapType << " map from '" << path
						  << "': " << e.what() << std::endl;
				texturePtr = nullptr;
				hasMapFlag = false;
			} catch (...) {
				std::cerr << "[MaterialPBR] Unknown error loading " << mapType << " map from '"
						  << path << "'." << std::endl;
				texturePtr = nullptr;
				hasMapFlag = false;
			}
		}

		// --- Texture Map Setters ---

		/**
		 * @brief Assign an albedo (diffuse) texture map.
		 */
		void SetAlbedoMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(albedoMap, hasAlbedoMap, "Albedo", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a normal map.
		 */
		void SetNormalMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(normalMap, hasNormalMap, "Normal", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign an ambient occlusion (AO) map.
		 */
		void SetAOMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(aoMap, hasAOMap, "AO", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a roughness map.
		 */
		void SetRoughnessMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(roughnessMap, hasRoughnessMap, "Roughness", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a metallic map.
		 */
		void SetMetallicMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(metallicMap, hasMetallicMap, "Metallic", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a specular map.
		 */
		void SetSpecularMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(specularMap, hasSpecularMap, "Specular", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign an emissive map.
		 */
		void SetEmissiveMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(emissiveMap, hasEmissiveMap, "Emissive", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign an opacity (alpha) map.
		 */
		void SetOpacityMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(opacityMap, hasOpacityMap, "Opacity", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a height (parallax) map.
		 */
		void SetHeightMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(heightMap, hasHeightMap, "Height", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a clearcoat map.
		 */
		void SetClearcoatMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(clearcoatMap, hasClearcoatMap, "Clearcoat", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign an anisotropy map.
		 */
		void SetAnisotropyMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(anisotropyMap, hasAnisotropyMap, "Anisotropy", path, targetWidth, targetHeight, algorithm);
		}

		/**
		 * @brief Assign a subsurface scattering (SSS) map.
		 */
		void SetSubsurfaceMap(const std::string &path, int targetWidth = 0, int targetHeight = 0, ResamplingAlgorithm algorithm = ResamplingAlgorithm::Bilinear) {
			LoadTextureMap(subsurfaceMap, hasSubsurfaceMap, "Subsurface", path, targetWidth, targetHeight, algorithm);
		}
	};

} // namespace Engine
