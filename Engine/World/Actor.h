/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Actor.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:27:39 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 23:03:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm> // Include for std::transform
#include <memory>
#include <type_traits>
#include <vector>

// Include the full definition instead of forward declaring
#include "World/Components/SceneComponent.h"

namespace Engine {

	class World;
	class ActorComponent;
	// No longer need to forward declare SceneComponent here

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
			// If the added component is a SceneComponent and not the root, attach it to the root by default
			if constexpr (std::is_base_of<SceneComponent, T>::value) {
				if (static_cast<SceneComponent *>(&ref) != m_RootComponent.get()) {
					// TODO: Uncomment this line after adding AttachToComponent(SceneComponent* parent) method to SceneComponent class.
					// static_cast<SceneComponent *>(&ref)->AttachToComponent(m_RootComponent.get());
				}
			}
			return ref;
		}

		// Get the first component of type T attached to this Actor (UE: GetComponentByClass)
		template <typename T>
		T *GetComponent() const {
			static_assert(std::is_base_of<ActorComponent, T>::value, "T must derive from ActorComponent");
			for (auto &comp : m_Components) {
				if (auto ptr = dynamic_cast<T *>(comp.get())) {
					return ptr;
				}
			}
			// Also check the root component if it's the requested type
			if (auto rootPtr = dynamic_cast<T *>(m_RootComponent.get())) {
				return rootPtr;
			}
			return nullptr;
		}

		// Get all components of type T attached to this Actor (UE: GetComponentsByClass)
		template <typename T>
		std::vector<T *> GetComponentsByClass() const {
			static_assert(std::is_base_of<ActorComponent, T>::value, "T must derive from ActorComponent");
			std::vector<T *> result;
			// Check owned components
			for (const auto &comp : m_Components) {
				if (T *ptr = dynamic_cast<T *>(comp.get())) {
					result.push_back(ptr);
				}
			}
			// Check root component separately if it matches the type T
			// (Avoid double-adding if root is also in m_Components, though it shouldn't be)
			if (T *rootPtr = dynamic_cast<T *>(m_RootComponent.get())) {
				// Ensure we don't add the root if it was somehow already added (unlikely)
				bool alreadyAdded = false;
				for (const auto *existingPtr : result) {
					if (existingPtr == rootPtr) {
						alreadyAdded = true;
						break;
					}
				}
				if (!alreadyAdded) {
					result.push_back(rootPtr);
				}
			}
			return result;
		}

		// Get all components attached to this Actor (UE: GetComponents)
		std::vector<ActorComponent *> GetComponents() const; // Declaration only

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