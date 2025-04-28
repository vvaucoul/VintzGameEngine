/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpotLightComponent.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:55:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SpotLightComponent.h"
#include "Renderer/Shaders/Shader.h" // Include Shader for SetupUniforms
#include "World/Actor.h"
#include "World/Components/SceneComponent.h" // Include SceneComponent for position/rotation
#include <cmath>							 // Include for cos
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp> // Include for quaternion rotation
#include <string>				  // Include for std::to_string

namespace Engine {

	SpotLightComponent::SpotLightComponent(Actor *owner, const glm::vec3 &color, float intensity, float cutOff, float outerCutOff, float constant, float linear, float quadratic)
		: PointLightComponent(owner, color, intensity, constant, linear, quadratic), // Call base constructor
		  m_CutOff(cutOff),															 // Corrected member name from m_InnerCutOff
		  m_OuterCutOff(outerCutOff) {
		// Add billboard component to the owner actor
		if (owner) {
			// Use SpotLight icon
			m_Billboard = &owner->AddComponent<BillboardComponent>("assets/billboards/Billboard_SpotLight.png", glm::vec2{0.5f, 0.5f});
		}
	}

	glm::vec3 SpotLightComponent::GetDirection() const {
		// Direction is the forward vector of the component's transform
		// This assumes SpotLightComponent (or its base PointLightComponent) inherits SceneComponent
		return GetForwardVector();
	}

	void SpotLightComponent::SetupUniforms(Shader &shader, int index) const {
		// Construct uniform name based on index (e.g., "spotLights[0].position")
		std::string baseName = "spotLights[" + std::to_string(index) + "]";

		shader.SetUniformVec3(baseName + ".position", GetOwner()->GetRootComponent()->GetWorldPosition());
		shader.SetUniformVec3(baseName + ".direction", GetDirection());
		shader.SetUniformVec3(baseName + ".color", m_Color);									  // Use inherited member
		shader.SetUniformFloat(baseName + ".intensity", m_Intensity);							  // Use inherited member
		shader.SetUniformFloat(baseName + ".cutOff", glm::cos(glm::radians(m_CutOff)));			  // Pass cosine of angle
		shader.SetUniformFloat(baseName + ".outerCutOff", glm::cos(glm::radians(m_OuterCutOff))); // Pass cosine of angle
		shader.SetUniformFloat(baseName + ".constant", m_Constant);
		shader.SetUniformFloat(baseName + ".linear", m_Linear);
		shader.SetUniformFloat(baseName + ".quadratic", m_Quadratic);
	}

} // namespace Engine
