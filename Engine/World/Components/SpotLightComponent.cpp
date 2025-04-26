/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpotLightComponent.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:31:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SpotLightComponent.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"					 // Include Actor header
#include "World/Components/SceneComponent.h" // Include SceneComponent header
#include <glm/gtc/matrix_transform.hpp>
#include <string> // For std::to_string

namespace Engine {

	SpotLightComponent::SpotLightComponent(Actor *owner, glm::vec3 color, float intensity, float cutOff, float outerCutOff, float constant, float linear, float quadratic)
		: ActorComponent(owner), m_Color(color), m_Intensity(intensity), m_CutOff(cutOff), m_OuterCutOff(outerCutOff), m_Constant(constant), m_Linear(linear), m_Quadratic(quadratic) {
	}

	SpotLightComponent::~SpotLightComponent() = default;

	void SpotLightComponent::Apply(Shader &shader, int index) const {
		glm::mat4 worldTransform = GetOwner()->GetRootComponent()->GetWorldTransform();
		glm::vec3 position		 = glm::vec3(worldTransform[3]);
		// Direction is the actor's forward vector (negative Z)
		glm::vec3 direction = -glm::normalize(glm::vec3(worldTransform[2]));

		std::string prefix = "spotLights[" + std::to_string(index) + "].";

		shader.SetUniformVec3(prefix + "position", position);
		shader.SetUniformVec3(prefix + "direction", direction);
		shader.SetUniformVec3(prefix + "color", m_Color);
		shader.SetUniformFloat(prefix + "intensity", m_Intensity);
		shader.SetUniformFloat(prefix + "cutOff", m_CutOff);
		shader.SetUniformFloat(prefix + "outerCutOff", m_OuterCutOff);
		shader.SetUniformFloat(prefix + "constant", m_Constant);
		shader.SetUniformFloat(prefix + "linear", m_Linear);
		shader.SetUniformFloat(prefix + "quadratic", m_Quadratic);
		// Add ambient/specular components if needed later
		// shader.SetUniformVec3(prefix + "ambient", m_Color * 0.05f);
		// shader.SetUniformVec3(prefix + "specular", glm::vec3(1.0f));
	}
} // namespace Engine
