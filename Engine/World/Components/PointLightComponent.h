/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PointLightComponent.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:47:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "World/Components/BillboardComponent.h" // Added include
#include "World/Components/LightComponent.h"

namespace Engine {

	class Actor;

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
		 * @param attenuationRadius Max distance the light affects. Default is 1000.0f.
		 * @param sourceRadius Radius of the light source sphere (for area lights/soft shadows). Default is 0.0f.
		 * @param softSourceRadius Controls shadow penumbra softness. Default is 0.0f.
		 * @param sourceLength Length for capsule lights. Default is 0.0f.
		 * @param constant Attenuation constant factor. Default is 1.0.
		 * @param linear Attenuation linear factor. Default is 0.09.
		 * @param quadratic Attenuation quadratic factor. Default is 0.032.
		 */
		PointLightComponent(Actor *owner,
							const glm::vec3 &color	= glm::vec3(1.0f),
							float intensity			= 1.0f,
							float attenuationRadius = 10.0f, // Added default
							float sourceRadius		= 0.0f,	 // Added default
							float softSourceRadius	= 0.0f,	 // Added default
							float sourceLength		= 0.0f,	 // Added default
							float constant			= 1.0f,	 // Default attenuation
							float linear			= 0.09f, // Default attenuation
							float quadratic			= 0.032f // Default attenuation
		);
		~PointLightComponent() override = default;

		/**
		 * @brief Sets up the point light uniforms in the shader array.
		 * @param shader The shader to set uniforms for.
		 * @param index The index in the shader's point light array.
		 */
		void SetupUniforms(Shader &shader, int index) const override;

		// --- Getters ---
		float GetAttenuationRadius() const { return m_AttenuationRadius; }
		float GetSourceRadius() const { return m_SourceRadius; }
		float GetSoftSourceRadius() const { return m_SoftSourceRadius; }
		float GetSourceLength() const { return m_SourceLength; }
		float GetConstant() const { return m_Constant; }
		float GetLinear() const { return m_Linear; }
		float GetQuadratic() const { return m_Quadratic; }

		// --- Setters ---
		void SetAttenuationRadius(float radius) { m_AttenuationRadius = radius; }
		void SetSourceRadius(float radius) { m_SourceRadius = radius; }
		void SetSoftSourceRadius(float radius) { m_SoftSourceRadius = radius; }
		void SetSourceLength(float length) { m_SourceLength = length; }
		void SetConstant(float constant) { m_Constant = constant; }
		void SetLinear(float linear) { m_Linear = linear; }
		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }

	private:
		// Reordered to match constructor initializer list in .cpp
		float m_AttenuationRadius;
		float m_SourceRadius;
		float m_SoftSourceRadius;
		float m_SourceLength;
		float m_Constant;
		float m_Linear;
		float m_Quadratic;
		BillboardComponent *m_Billboard = nullptr; ///< Billboard for editor/debug visualization.
	};

} // namespace Engine
