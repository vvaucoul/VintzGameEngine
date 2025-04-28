/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpotLightComponent.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:47:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "World/Components/BillboardComponent.h"  // Added include
#include "World/Components/PointLightComponent.h" // SpotLight often inherits attenuation from PointLight

namespace Engine {

	class Actor;

	/**
	 * @class SpotLightComponent
	 * @brief Represents a spotlight source emitting light in a specific direction cone.
	 */
	class SpotLightComponent : public PointLightComponent {
	public:
		/**
		 * @brief Constructor.
		 * @param owner The Actor that owns this component.
		 * @param color The color of the light. Default is white.
		 * @param intensity The brightness of the light. Default is 1.0.
		 * @param cutOff Inner cone angle (degrees). Default is 12.5.
		 * @param outerCutOff Outer cone angle (degrees). Default is 17.5.
		 * @param constant Attenuation constant factor. Default is 1.0.
		 * @param linear Attenuation linear factor. Default is 0.09.
		 * @param quadratic Attenuation quadratic factor. Default is 0.032.
		 */
		SpotLightComponent(Actor *owner,
						   const glm::vec3 &color = glm::vec3(1.0f),
						   float intensity		  = 1.0f,
						   float cutOff			  = 12.5f,
						   float outerCutOff	  = 17.5f,
						   float constant		  = 1.0f,
						   float linear			  = 0.09f,
						   float quadratic		  = 0.032f);
		~SpotLightComponent() override = default;

		/**
		 * @brief Sets up the spot light uniforms in the shader array.
		 * @param shader The shader to set uniforms for.
		 * @param index The index in the shader's spot light array.
		 */
		void SetupUniforms(Shader &shader, int index) const override;

		/**
		 * @brief Gets the direction the spotlight is pointing based on the owner Actor's rotation.
		 * @return The normalized direction vector.
		 */
		glm::vec3 GetDirection() const;

		// --- Getters ---
		float GetCutOff() const { return m_CutOff; }
		float GetOuterCutOff() const { return m_OuterCutOff; }
		float GetConstant() const { return m_Constant; }
		float GetLinear() const { return m_Linear; }
		float GetQuadratic() const { return m_Quadratic; }

		// --- Setters ---
		void SetCutOff(float cutOff) { m_CutOff = cutOff; }
		void SetOuterCutOff(float outerCutOff) { m_OuterCutOff = outerCutOff; }
		void SetConstant(float constant) { m_Constant = constant; }
		void SetLinear(float linear) { m_Linear = linear; }
		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }

		// Color and Intensity are inherited

	private:
		float m_CutOff;		 // Inner angle (degrees)
		float m_OuterCutOff; // Outer angle (degrees)
		float m_Constant;
		float m_Linear;
		float m_Quadratic;
		BillboardComponent *m_Billboard = nullptr; ///< Billboard for editor/debug visualization.
	};

} // namespace Engine
