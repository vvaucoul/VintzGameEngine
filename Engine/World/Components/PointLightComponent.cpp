/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PointLightComponent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:55:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PointLightComponent.h"
#include "Renderer/Shaders/Shader.h" // Include Shader for SetupUniforms
#include "World/Actor.h"
#include "World/Components/SceneComponent.h" // Include SceneComponent for position
#include <string>							 // Include for std::to_string

namespace Engine {

	PointLightComponent::PointLightComponent(Actor *owner, const glm::vec3 &color, float intensity, float attenuationRadius, float sourceRadius, float softSourceRadius, float sourceLength, float constant, float linear, float quadratic)
		: LightComponent(owner, color, intensity), // Call base class constructor
		  m_AttenuationRadius(attenuationRadius),  // Init new member
		  m_SourceRadius(sourceRadius),			   // Init new member
		  m_SoftSourceRadius(softSourceRadius),	   // Init new member
		  m_SourceLength(sourceLength),			   // Init new member
		  m_Constant(constant),
		  m_Linear(linear),
		  m_Quadratic(quadratic) {
	}

	void PointLightComponent::SetupUniforms(Shader &shader, int index) const {
		// Construct uniform name based on index (e.g., "pointLights[0].position")
		std::string baseName = "pointLights[" + std::to_string(index) + "]";

		shader.SetUniformVec3(baseName + ".position", GetOwner()->GetRootComponent()->GetWorldPosition());
		shader.SetUniformVec3(baseName + ".color", m_Color);		  // Use inherited member
		shader.SetUniformFloat(baseName + ".intensity", m_Intensity); // Use inherited member
		shader.SetUniformFloat(baseName + ".constant", m_Constant);
		shader.SetUniformFloat(baseName + ".linear", m_Linear);
		shader.SetUniformFloat(baseName + ".quadratic", m_Quadratic);
		// Set new uniforms
		shader.SetUniformFloat(baseName + ".attenuationRadius", m_AttenuationRadius);
		shader.SetUniformFloat(baseName + ".sourceRadius", m_SourceRadius);
		shader.SetUniformFloat(baseName + ".softSourceRadius", m_SoftSourceRadius);
		shader.SetUniformFloat(baseName + ".sourceLength", m_SourceLength);
	}

} // namespace Engine
