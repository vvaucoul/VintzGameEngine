/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectionalLightComponent.h                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:25:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:47:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LightComponent.h"						 // Include the new base class
#include "World/Components/BillboardComponent.h" // Added include

namespace Engine {

	/**
	 * @class DirectionalLightComponent
	 * @brief Represents a directional light source (like the sun).
	 */
	class DirectionalLightComponent : public LightComponent { // Inherit from LightComponent
	public:
		/**
		 * @brief Constructor.
		 * @param owner The Actor that owns this component.
		 * @param color The color of the light. Default is white.
		 * @param intensity The brightness of the light. Default is 1.0.
		 */
		DirectionalLightComponent(Actor *owner, const glm::vec3 &color = {1.0f, 1.0f, 1.0f}, float intensity = 1.0f);
		~DirectionalLightComponent() override = default;

		/**
		 * @brief Sets up the directional light uniforms in the shader.
		 * @param shader The shader to set uniforms for.
		 * @param index Not used for directional light (only one).
		 */
		void SetupUniforms(Shader &shader, int index) const override;

		/**
		 * @brief Gets the direction of the light based on the owner Actor's rotation.
		 * @return The normalized direction vector.
		 */
		glm::vec3 GetDirection() const;

	private:
		BillboardComponent *m_Billboard = nullptr; ///< Billboard for editor/debug visualization.

		// Color and Intensity are now inherited from LightComponent
	};

} // namespace Engine
