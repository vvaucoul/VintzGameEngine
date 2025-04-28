/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultMaterial.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:20:13 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:41:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderer/Materials/MaterialPBR.h" // MaterialPBR definition
#include <memory>							// std::shared_ptr

namespace Engine {

	/**
	 * @class DefaultMaterial
	 * @brief Provides access to a singleton instance of a default physically-based material.
	 *
	 * The default material uses physically plausible PBR values:
	 *   - Albedo: White (1.0, 1.0, 1.0)
	 *   - Metallic: 0.0 (dielectric)
	 *   - Roughness: 0.5 (medium roughness)
	 *   - Ambient Occlusion: 1.0 (fully lit)
	 *   - Specular: 0.5 (balanced)
	 *   - No texture maps assigned
	 *
	 * @return Shared pointer to the immutable default MaterialPBR instance.
	 */
	std::shared_ptr<MaterialPBR> GetDefaultMaterial();

} // namespace Engine
