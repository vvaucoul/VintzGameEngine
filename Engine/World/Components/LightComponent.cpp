/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LightComponent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:48:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:48:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/LightComponent.h"
// Other necessary includes...

namespace Engine {

	LightComponent::LightComponent(Actor *owner, const glm::vec3 &color, float intensity)
		: SceneComponent(owner), // Call the base SceneComponent constructor
		  m_Color(color),
		  m_Intensity(intensity) {
		// Initialization specific to LightComponent, if any
	}

	// ... other LightComponent method implementations ...

} // namespace Engine
