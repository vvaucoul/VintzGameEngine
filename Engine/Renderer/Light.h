/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:24:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:25:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <glm/glm.hpp>

namespace Engine {

	// Directional Light
	struct DirLight {
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
	};

	// Point Light (max 4)
	struct PointLight {
		glm::vec3 position;
		glm::vec3 color;
		float intensity;
		float constant, linear, quadratic;
	};

	// Spot Light (max 4)
	struct SpotLight {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
		float cutOff, outerCutOff;
		float constant, linear, quadratic;
	};

} // namespace Engine
