/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Actor.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:26:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 10:57:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Actor.h"
#include "World/Components/SceneComponent.h"

namespace Engine {

	Actor::Actor(uint32_t id, World *world)
		: m_ID(id), m_World(world), m_RootComponent(std::make_unique<SceneComponent>(this)) {
	}

	Actor::~Actor() = default;

	uint32_t Actor::GetID() const {
		return m_ID;
	}

	SceneComponent *Actor::GetRootComponent() const {
		return m_RootComponent.get();
	}

	void Actor::Tick(float deltaTime) {
		// Tick all components (including the root component implicitly via GetComponents)
		for (ActorComponent *comp : GetComponents()) {
			if (comp) { // Basic null check
				comp->Tick(deltaTime);
			}
		}
	}

} // namespace Engine