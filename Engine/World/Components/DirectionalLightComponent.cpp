/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectionalLightComponent.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:25:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:30:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectionalLightComponent.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"					 // Include Actor header
#include "World/Components/SceneComponent.h" // Include SceneComponent header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> // For glm::mat4_cast

namespace Engine {

	DirectionalLightComponent::DirectionalLightComponent(Actor *owner, glm::vec3 color, float intensity)
		: ActorComponent(owner), m_Color(color), m_Intensity(intensity) {
	}

	DirectionalLightComponent::~DirectionalLightComponent() = default;

	void DirectionalLightComponent::Apply(Shader &shader) const {
		// Direction is determined by the owning actor's forward vector
		// Get the world transform of the root component
		glm::mat4 worldTransform = GetOwner()->GetRootComponent()->GetWorldTransform();

		// Extract the forward vector (negative Z-axis in view space) from the rotation part of the matrix
		// Assuming standard convention where +Z is forward locally, transform it to world space.
		// The third column (index 2) of the world matrix represents the local Z-axis in world space.
		// We negate it because light direction is usually defined as "coming from" the source.
		glm::vec3 direction = -glm::normalize(glm::vec3(worldTransform[2]));

		shader.SetUniformVec3("dirLight.direction", direction);
		shader.SetUniformVec3("dirLight.color", m_Color);
		shader.SetUniformFloat("dirLight.intensity", m_Intensity);
		// Add ambient/specular components if needed later
		// shader.SetUniformVec3("dirLight.ambient", m_Color * 0.1f); // Example ambient
		// shader.SetUniformVec3("dirLight.specular", glm::vec3(1.0f)); // Example specular
	}
} // namespace Engine
