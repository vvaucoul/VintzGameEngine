/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ActorComponent.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:26:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 19:59:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	class Actor;

	class ActorComponent {
	public:
		explicit ActorComponent(Actor *owner);
		virtual ~ActorComponent();

		// Called every frame
		virtual void Tick([[maybe_unused]] float deltaTime) {}

		Actor *GetOwner() const;

	private:
		Actor *m_Owner;
	};

} // namespace Engine