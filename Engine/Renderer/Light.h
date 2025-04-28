/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:24:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:27:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>

/**
 * @file Light.h
 * @brief Defines basic light types for the renderer (directional, point, spot).
 */

namespace Engine {

	/**
	 * @struct DirLight
	 * @brief Represents a directional light (e.g., sunlight).
	 */
	struct DirLight {
		glm::vec3 direction; ///< Light direction (normalized)
		glm::vec3 color;	 ///< RGB color (linear space)
		float intensity;	 ///< Light intensity multiplier
	};

	/**
	 * @struct PointLight
	 * @brief Represents a point light source (emits in all directions).
	 * @note Typically limited to 4 active point lights for performance.
	 */
	struct PointLight {
		glm::vec3 position; ///< World-space position
		glm::vec3 color;	///< RGB color (linear space)
		float intensity;	///< Light intensity multiplier

		// Attenuation factors (see: https://learnopengl.com/Lighting/Light-casters)
		float constant;	 ///< Constant attenuation
		float linear;	 ///< Linear attenuation
		float quadratic; ///< Quadratic attenuation
	};

	/**
	 * @struct SpotLight
	 * @brief Represents a spot light (cone-shaped emission).
	 * @note Typically limited to 4 active spot lights for performance.
	 */
	struct SpotLight {
		glm::vec3 position;	 ///< World-space position
		glm::vec3 direction; ///< Direction the spotlight is facing (normalized)
		glm::vec3 color;	 ///< RGB color (linear space)
		float intensity;	 ///< Light intensity multiplier

		float cutOff;	   ///< Inner cutoff angle (cosine, radians)
		float outerCutOff; ///< Outer cutoff angle (cosine, radians)

		// Attenuation factors
		float constant;	 ///< Constant attenuation
		float linear;	 ///< Linear attenuation
		float quadratic; ///< Quadratic attenuation
	};

} // namespace Engine
