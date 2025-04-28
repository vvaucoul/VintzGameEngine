/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultMaterial.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:20:13 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:21:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderer/Materials/MaterialPBR.h" // Include MaterialPBR definition
#include <memory>							// For std::shared_ptr

namespace Engine {

	/**
	 * @brief Gets a shared pointer to the singleton default PBR material.
	 *
	 * This material has default properties (e.g., white albedo, non-metallic, medium roughness).
	 * @return A shared pointer to the default MaterialPBR instance.
	 */
	std::shared_ptr<MaterialPBR> GetDefaultMaterial();

} // namespace Engine
