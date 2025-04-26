/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Actor.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:27:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 13:27:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>
#include <type_traits>
#include <vector>

namespace Engine {

	class World;
	class ActorComponent;
	class SceneComponent;

	class Actor {
	public:
		Actor(uint32_t id, World *world);
		~Actor();

		uint32_t GetID() const;

		// Add a component of type T to this Actor
		template <typename T, typename... Args>
		T &AddComponent(Args &&...args) {
			static_assert(std::is_base_of<ActorComponent, T>::value, "T must derive from ActorComponent");
			auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
			T &ref	  = *comp;
			m_Components.push_back(std::move(comp));
			return ref;
		}

		// Get the first component of type T attached to this Actor
		template <typename T>
		T *GetComponent() const {
			for (auto &comp : m_Components) {
				if (auto ptr = dynamic_cast<T *>(comp.get()))
					return ptr;
			}
			return nullptr;
		}

		// Access the root SceneComponent (transform & hierarchy)
		SceneComponent *GetRootComponent() const;

		// Called every frame
		void Tick(float deltaTime);

	private:
		uint32_t m_ID;
		World *m_World;
		std::unique_ptr<SceneComponent> m_RootComponent;
		std::vector<std::unique_ptr<ActorComponent>> m_Components;
	};

} // namespace Engine