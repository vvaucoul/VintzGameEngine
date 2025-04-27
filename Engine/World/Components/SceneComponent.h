/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneComponent.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:15:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 12:01:35 by vvaucoul         ###   ########.fr       */
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

	class SceneComponent : public ActorComponent {
		DECLARE_REFLECTABLE(SceneComponent) // Add reflection declaration
	public:
		SceneComponent(Actor *owner);
		~SceneComponent() override;

		// Transform setters
		void SetPosition(const glm::vec3 &position);
		void SetRotation(const glm::vec3 &eulerAngles);
		void SetScale(const glm::vec3 &scale);

		// Hierarchy
		void AttachTo(SceneComponent *parent);
		void AddChild(SceneComponent *child);

		// Get transforms
		glm::mat4 GetLocalTransform() const;
		glm::mat4 GetWorldTransform() const;
		glm::vec3 GetWorldPosition() const;
		glm::quat GetWorldRotationQuat() const; // Add this declaration

		// Apply a full transform (decomposed into position, rotation, scale)
		void SetTransform(const glm::mat4 &transform);

		// Called every frame
		void Tick(float deltaTime) override;

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;

		SceneComponent *m_Parent;
		std::vector<SceneComponent *> m_Children;

		void UpdateWorldTransform();
	};

} // namespace Engine