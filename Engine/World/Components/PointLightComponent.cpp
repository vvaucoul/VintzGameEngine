/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PointLightComponent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:31:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PointLightComponent.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"					 // Include Actor header
#include "World/Components/SceneComponent.h" // Include SceneComponent header
#include <glm/gtc/matrix_transform.hpp>
#include <string> // For std::to_string

namespace Engine {

	PointLightComponent::PointLightComponent(Actor *owner, glm::vec3 color, float intensity, float constant, float linear, float quadratic)
		: ActorComponent(owner), m_Color(color), m_Intensity(intensity), m_Constant(constant), m_Linear(linear), m_Quadratic(quadratic) {
	}

	PointLightComponent::~PointLightComponent() = default;

	void PointLightComponent::Apply(Shader &shader, int index) const {
		// Position is determined by the owning actor's root component world position
		glm::vec3 position = glm::vec3(GetOwner()->GetRootComponent()->GetWorldTransform()[3]);

		std::string prefix = "pointLights[" + std::to_string(index) + "].";

		shader.SetUniformVec3(prefix + "position", position);
		shader.SetUniformVec3(prefix + "color", m_Color);
		shader.SetUniformFloat(prefix + "intensity", m_Intensity);
		shader.SetUniformFloat(prefix + "constant", m_Constant);
		shader.SetUniformFloat(prefix + "linear", m_Linear);
		shader.SetUniformFloat(prefix + "quadratic", m_Quadratic);
		// Add ambient/specular components if needed later
		// shader.SetUniformVec3(prefix + "ambient", m_Color * 0.05f);
		// shader.SetUniformVec3(prefix + "specular", glm::vec3(1.0f));
	}
} // namespace Engine
