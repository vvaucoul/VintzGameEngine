/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PointLightComponent.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:26:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:29:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "World/ActorComponent.h"
#include <glm/glm.hpp>

namespace Engine {
	class Shader;

	class PointLightComponent : public ActorComponent {
	public:
		PointLightComponent(Actor *owner,
							glm::vec3 color = {1, 1, 1},
							float intensity = 1.0f,
							float constant	= 1.0f,
							float linear	= 0.09f,
							float quadratic = 0.032f);
		~PointLightComponent() override;

		void Tick(float) override {}
		void Apply(Shader &shader, int index) const;

	private:
		glm::vec3 m_Color;
		float m_Intensity;
		float m_Constant, m_Linear, m_Quadratic;
	};
}
