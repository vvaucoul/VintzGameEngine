/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultMaterial.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:20:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:21:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Materials/DefaultMaterial.h"

namespace Engine {

	std::shared_ptr<MaterialPBR> GetDefaultMaterial() {
		// Static local variable ensures singleton instance is created only once
		static std::shared_ptr<MaterialPBR> defaultMaterial = nullptr;

		if (!defaultMaterial) {
			defaultMaterial = std::make_shared<MaterialPBR>();
			// Set default PBR values
			defaultMaterial->albedoColor = {1.0f, 1.0f, 1.0f}; // White
			defaultMaterial->metallic	 = 0.0f;			   // Non-metallic
			defaultMaterial->roughness	 = 0.5f;			   // Medium roughness
			defaultMaterial->ao			 = 1.0f;			   // Full ambient occlusion
			defaultMaterial->specular	 = 0.5f;			   // Default specular

			// Ensure all texture flags are false initially
			defaultMaterial->hasAlbedoMap	 = false;
			defaultMaterial->hasNormalMap	 = false;
			defaultMaterial->hasAOMap		 = false;
			defaultMaterial->hasRoughnessMap = false;
			defaultMaterial->hasMetallicMap	 = false;
			defaultMaterial->hasSpecularMap	 = false;
		}

		return defaultMaterial;
	}

} // namespace Engine
