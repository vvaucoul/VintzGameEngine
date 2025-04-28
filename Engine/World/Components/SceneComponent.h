/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneComponent.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:15:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:54:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Core/Reflection/ReflectionMacros.h" // Include reflection macros
#include "World/ActorComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> // Include for quaternion
#include <vector>

namespace Engine {

	// Forward declare Actor to avoid circular dependency if ActorComponent includes Actor.h
	class Actor;

	// Correct the class name here
	class SceneComponent : public ActorComponent {
		DECLARE_REFLECTABLE(SceneComponent) // Ensure this matches the class name
	public:
		// Constructor and Destructor should use the correct class name
		SceneComponent(Actor *owner);
		~SceneComponent() override;

		// Transform setters
		void SetPosition(const glm::vec3 &position);
		void SetRotation(const glm::vec3 &eulerAngles);
		void SetScale(const glm::vec3 &scale);

		// Hierarchy
		void AttachTo(SceneComponent *parent); // Parameter type should match class name
		void AddChild(SceneComponent *child);  // Parameter type should match class name

		// Get transforms
		glm::mat4 GetLocalTransform() const;
		glm::mat4 GetWorldTransform() const;
		glm::vec3 GetWorldPosition() const;
		glm::quat GetWorldRotationQuat() const; // Add this declaration

		// Apply a full transform (decomposed into position, rotation, scale)
		void SetTransform(const glm::mat4 &transform);

		// Called every frame
		void Tick(float deltaTime) override;

		// --- Transform Getters ---
		/**
		 * @brief Gets the forward direction vector (+X) in world space.
		 * @return Normalized forward vector.
		 */
		glm::vec3 GetForwardVector() const;

		/**
		 * @brief Gets the right direction vector (+Y) in world space.
		 * @return Normalized right vector.
		 */
		glm::vec3 GetRightVector() const;

		/**
		 * @brief Gets the up direction vector (+Z) in world space.
		 * @return Normalized up vector.
		 */
		glm::vec3 GetUpVector() const;

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;

		SceneComponent *m_Parent;				  // Member type should match class name
		std::vector<SceneComponent *> m_Children; // Template argument should match class name

		void UpdateWorldTransform();
	};

} // namespace Engine