/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultMaterial.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:20:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:52:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Materials/DefaultMaterial.h"

namespace Engine {

	std::shared_ptr<MaterialPBR> GetDefaultMaterial() {
		// Singleton pattern: create the default material only once
		static std::shared_ptr<MaterialPBR> defaultMaterial = nullptr;

		if (!defaultMaterial) {
			defaultMaterial = std::make_shared<MaterialPBR>();

			// --- Set physically reasonable PBR defaults ---
			defaultMaterial->albedoColor = {1.0f, 1.0f, 1.0f}; // White diffuse
			defaultMaterial->metallic	 = 0.0f;			   // Dielectric
			defaultMaterial->roughness	 = 0.5f;			   // Medium roughness
			defaultMaterial->ao			 = 1.0f;			   // Full ambient occlusion
			defaultMaterial->specular	 = 0.5f;			   // Balanced specular

			// --- Initialize Extended Feature Defaults ---
			defaultMaterial->emissiveColor		 = {0.0f, 0.0f, 0.0f}; // Not emissive
			defaultMaterial->opacity			 = 1.0f;			   // Opaque
			defaultMaterial->parallaxScale		 = 0.0f;			   // No parallax effect (set to > 0 to enable)
			defaultMaterial->clearcoat			 = 0.0f;			   // No clearcoat
			defaultMaterial->clearcoatRoughness	 = 0.1f;
			defaultMaterial->sheenColor			 = {0.0f, 0.0f, 0.0f}; // No sheen
			defaultMaterial->sheenRoughness		 = 0.3f;
			defaultMaterial->anisotropy			 = 0.0f; // Isotropic
			defaultMaterial->anisotropyDirection = {1.0f, 0.0f, 0.0f};
			defaultMaterial->subsurface			 = 0.0f; // No SSS
			defaultMaterial->subsurfaceColor	 = {1.0f, 1.0f, 1.0f};

			// No texture maps by default
			defaultMaterial->hasAlbedoMap	  = false;
			defaultMaterial->hasNormalMap	  = false;
			defaultMaterial->hasAOMap		  = false;
			defaultMaterial->hasRoughnessMap  = false;
			defaultMaterial->hasMetallicMap	  = false;
			defaultMaterial->hasSpecularMap	  = false;
			defaultMaterial->hasEmissiveMap	  = false;
			defaultMaterial->hasOpacityMap	  = false;
			defaultMaterial->hasHeightMap	  = false;
			defaultMaterial->hasClearcoatMap  = false;
			defaultMaterial->hasAnisotropyMap = false;
			defaultMaterial->hasSubsurfaceMap = false;

			// --- Other Properties ---
			defaultMaterial->doubleSided = false; // Default to back-face culling
		}

		return defaultMaterial;
	}

} // namespace Engine
