/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectionalLightComponent.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:25:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:51:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectionalLightComponent.h"
#include "Renderer/Shader.h" // Include Shader for SetupUniforms
#include "World/Actor.h"
#include "World/Components/SceneComponent.h" // Include SceneComponent for rotation
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp> // Include for quaternion rotation

namespace Engine {

	DirectionalLightComponent::DirectionalLightComponent(Actor *owner, const glm::vec3 &color, float intensity)
		: LightComponent(owner, color, intensity) { // Call base class constructor
	}

	glm::vec3 DirectionalLightComponent::GetDirection() const {
		// Calculate direction based on the owning actor's rotation
		// Assuming default direction is -Z (forward)
		glm::quat rotation = GetOwner()->GetRootComponent()->GetWorldRotationQuat();
		return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void DirectionalLightComponent::SetupUniforms(Shader &shader, [[maybe_unused]] int index) const {
		shader.SetUniformVec3("dirLight.direction", GetDirection());
		shader.SetUniformVec3("dirLight.color", m_Color);		   // Use inherited member
		shader.SetUniformFloat("dirLight.intensity", m_Intensity); // Use inherited member
	}

} // namespace Engine
