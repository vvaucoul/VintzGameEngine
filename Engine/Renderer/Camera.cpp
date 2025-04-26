/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:08:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:55:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Camera.h"
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

namespace Engine {

	Camera::Camera(const glm::vec3 &initialPosition, float fov, float aspectRatio, float nearClip, float farClip)
		: m_Position(initialPosition),
		  m_Front(0.0f, 0.0f, -1.0f), // Initial direction
		  m_Up(0.0f, 1.0f, 0.0f),
		  m_Pitch(0.0f),
		  m_Yaw(-90.0f), // Pointing down the negative Z-axis initially
		  m_MovementSpeed(2.5f),
		  m_MouseSensitivity(0.0025f),
		  m_FOV(fov),
		  m_AspectRatio(aspectRatio),
		  m_NearClip(nearClip),
		  m_FarClip(farClip) {
		RecalculateView(); // Calculate initial Right vector and View matrix
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void Camera::ProcessKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down) {
		float velocity = m_MovementSpeed * deltaTime;
		if (forward) m_Position += m_Front * velocity;
		if (backward) m_Position -= m_Front * velocity;
		if (left) m_Position -= m_Right * velocity;
		if (right) m_Position += m_Right * velocity;
		if (up) m_Position += glm::vec3(0, 1, 0) * velocity;
		if (down) m_Position -= glm::vec3(0, 1, 0) * velocity;
		RecalculateView();
	}

	void Camera::ProcessMouseMovement(float deltaX, float deltaY) {

		deltaX *= m_MouseSensitivity;
		deltaY *= m_MouseSensitivity;

		m_Yaw += deltaX;
		m_Pitch += deltaY; // Note: deltaY is already inverted in Application::MouseCallback

		// Clamp pitch to avoid flipping
		m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

		RecalculateView();
	}

	void Camera::SetAspectRatio(float aspectRatio) {
		m_AspectRatio	   = aspectRatio;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void Camera::RecalculateView() {
		glm::vec3 front;
		front.x		 = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y		 = sin(glm::radians(m_Pitch));
		front.z		 = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front		 = glm::normalize(front);
		m_Right		 = glm::normalize(glm::cross(m_Front, glm::vec3(0, 1, 0)));
		m_Up		 = glm::normalize(glm::cross(m_Right, m_Front));
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

} // namespace Engine
