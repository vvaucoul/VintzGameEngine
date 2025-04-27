/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Actor.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:26:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 23:03:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Actor.h"
#include "World/Components/SceneComponent.h"
#include <algorithm> // Required for std::transform
#include <iterator>	 // Required for std::back_inserter

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

	// Get all components attached to this Actor (UE: GetComponents)
	std::vector<ActorComponent *> Actor::GetComponents() const {
		std::vector<ActorComponent *> result;
		result.reserve(m_Components.size() + 1); // Reserve space for owned + root
		// Add the root component first, casting it to the base type
		result.push_back(static_cast<ActorComponent *>(m_RootComponent.get()));
		// Add pointers to owned components
		std::transform(m_Components.begin(), m_Components.end(), std::back_inserter(result), [](const std::unique_ptr<ActorComponent> &comp) { return comp.get(); });
		return result;
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