/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpotLightComponent.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:29:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "World/ActorComponent.h"
#include <glm/glm.hpp>

namespace Engine {
	class Shader;

	class SpotLightComponent : public ActorComponent {
	public:
		SpotLightComponent(Actor *owner,
						   glm::vec3 color	 = {1, 1, 1},
						   float intensity	 = 1.0f,
						   float cutOff		 = glm::cos(glm::radians(12.5f)),
						   float outerCutOff = glm::cos(glm::radians(15.0f)),
						   float constant	 = 1.0f,
						   float linear		 = 0.09f,
						   float quadratic	 = 0.032f);
		~SpotLightComponent() override;

		void Tick(float) override {}
		void Apply(Shader &shader, int index) const;

	private:
		glm::vec3 m_Color;
		float m_Intensity;
		float m_CutOff, m_OuterCutOff;
		float m_Constant, m_Linear, m_Quadratic;
	};
}
