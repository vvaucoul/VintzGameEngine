/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectionalLightComponent.h                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 20:25:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:29:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "World/ActorComponent.h"
#include <glm/glm.hpp>

namespace Engine {
	class Shader;

	class DirectionalLightComponent : public ActorComponent {
	public:
		DirectionalLightComponent(Actor *owner,
								  glm::vec3 color = {1, 1, 1},
								  float intensity = 1.0f);
		~DirectionalLightComponent() override;

		void Tick(float) override {}
		void Apply(Shader &shader) const;

	private:
		glm::vec3 m_Color;
		float m_Intensity;
	};
}
