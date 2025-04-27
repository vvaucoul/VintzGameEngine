/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PointLightComponent.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:51:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LightComponent.h" // Include the new base class

namespace Engine {

	/**
	 * @class PointLightComponent
	 * @brief Represents a point light source emitting light in all directions.
	 */
	class PointLightComponent : public LightComponent { // Inherit from LightComponent
	public:
		/**
		 * @brief Constructor.
		 * @param owner The Actor that owns this component.
		 * @param color The color of the light. Default is white.
		 * @param intensity The brightness of the light. Default is 1.0.
		 * @param constant Attenuation constant factor. Default is 1.0.
		 * @param linear Attenuation linear factor. Default is 0.09.
		 * @param quadratic Attenuation quadratic factor. Default is 0.032.
		 */
		PointLightComponent(Actor *owner,
							const glm::vec3 &color = glm::vec3(1.0f),
							float intensity		   = 1.0f,
							float constant		   = 1.0f,
							float linear		   = 0.09f,
							float quadratic		   = 0.032f);

		/**
		 * @brief Sets up the point light uniforms in the shader array.
		 * @param shader The shader to set uniforms for.
		 * @param index The index in the shader's point light array.
		 */
		void SetupUniforms(Shader &shader, int index) const override;

		// --- Getters ---
		float GetConstant() const { return m_Constant; }
		float GetLinear() const { return m_Linear; }
		float GetQuadratic() const { return m_Quadratic; }

		// --- Setters ---
		void SetConstant(float constant) { m_Constant = constant; }
		void SetLinear(float linear) { m_Linear = linear; }
		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }

		// Color and Intensity are inherited

	private:
		float m_Constant;
		float m_Linear;
		float m_Quadratic;
	};

} // namespace Engine
