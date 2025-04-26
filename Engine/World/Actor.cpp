/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Actor.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:26:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 13:27:52 by vvaucoul         ###   ########.fr       */
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
		// Update root (if needed)
		m_RootComponent->Tick(deltaTime);
		// Update all other components
		for (auto &comp : m_Components)
			comp->Tick(deltaTime);
	}

} // namespace Engine