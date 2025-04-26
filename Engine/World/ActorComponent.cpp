/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ActorComponent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:26:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 19:58:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/ActorComponent.h"
#include "World/Actor.h"

namespace Engine {

	ActorComponent::ActorComponent(Actor *owner)
		: m_Owner(owner) {
	}

	ActorComponent::~ActorComponent() = default;

	Actor *ActorComponent::GetOwner() const {
		return m_Owner;
	}

} // namespace Engine