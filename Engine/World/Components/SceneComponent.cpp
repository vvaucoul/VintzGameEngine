/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneComponent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:28:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 12:04:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/SceneComponent.h"
#include "Core/Reflection/ReflectionMacros.h" // Ensure macros are included
#include "Core/Reflection/ReflectionRegistry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

// Explicitly specialize GetTypeName for SceneComponent within the correct namespace
namespace Engine::Reflection {
	template <>
	const char *ReflectionRegistry::GetTypeName<Engine::SceneComponent>() { return "SceneComponent"; }
} // namespace Engine::Reflection

namespace Engine {

	// Use reflection macros instead of manual registration
	BEGIN_DEFINE_TYPE(SceneComponent)
	// Pass ClassName to DEFINE_FIELD
	DEFINE_FIELD(SceneComponent, m_Position)
	DEFINE_FIELD(SceneComponent, m_Rotation)
	DEFINE_FIELD(SceneComponent, m_Scale)
	END_DEFINE_TYPE(SceneComponent)

	SceneComponent::SceneComponent(Actor *owner)
		: ActorComponent(owner), m_Position(0.0f), m_Rotation(0.0f), m_Scale(1.0f), m_Parent(nullptr) {
	}

	SceneComponent::~SceneComponent() = default;

	void SceneComponent::SetPosition(const glm::vec3 &position) {
		m_Position = position;
	}

	void SceneComponent::SetRotation(const glm::vec3 &eulerAngles) {
		m_Rotation = eulerAngles;
	}

	void SceneComponent::SetScale(const glm::vec3 &scale) {
		m_Scale = scale;
	}

	void SceneComponent::AttachTo(SceneComponent *parent) {
		if (m_Parent) {
			// detach from old parent
			// TODO: remove from parent's children
		}
		m_Parent = parent;
		if (parent)
			parent->AddChild(this);
	}

	void SceneComponent::AddChild(SceneComponent *child) {
		m_Children.push_back(child);
		child->m_Parent = this;
	}

	glm::mat4 SceneComponent::GetLocalTransform() const {
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_Position);
		glm::mat4 rot	= glm::toMat4(glm::quat(glm::radians(m_Rotation)));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);
		return trans * rot * scale;
	}

	glm::mat4 SceneComponent::GetWorldTransform() const {
		if (m_Parent)
			return m_Parent->GetWorldTransform() * GetLocalTransform();
		return GetLocalTransform();
	}

	glm::vec3 SceneComponent::GetWorldPosition() const {
		return glm::vec3(GetWorldTransform()[3]);
	}

	glm::quat SceneComponent::GetWorldRotationQuat() const {
		glm::quat parentRotation = (m_Parent) ? m_Parent->GetWorldRotationQuat() : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::quat localRotation	 = glm::quat(glm::radians(m_Rotation));
		return parentRotation * localRotation;
	}

	void SceneComponent::Tick(float deltaTime) {
		// Optionally update children transform logic
		for (auto child : m_Children)
			child->Tick(deltaTime);
	}

	void SceneComponent::SetTransform(const glm::mat4 &transform) {
		glm::vec3 translation, scale, skew;
		glm::vec4 perspective;
		glm::quat orientation;

		glm::decompose(transform, scale, orientation, translation, skew, perspective);

		m_Position = translation;
		m_Scale	   = scale;
		m_Rotation = glm::degrees(glm::eulerAngles(orientation));
	}

} // namespace Engine