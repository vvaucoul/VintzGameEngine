/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:07:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 22:57:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	class Camera {
	public:
		// Modified constructor to accept initial position
		Camera(const glm::vec3 &initialPosition, float fov, float aspectRatio, float nearClip, float farClip);
		~Camera() = default;

		// Movement
		void ProcessKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down);
		void ProcessMouseMovement(float deltaX, float deltaY);

		// Get matrices & position
		const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::vec3 &GetPosition() const { return m_Position; }

		// Adjust aspect on resize
		void SetAspectRatio(float aspectRatio);

	private:
		void RecalculateView();

	private:
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::vec3 m_Position;
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Right;

		float m_Pitch;
		float m_Yaw;
		float m_MovementSpeed;
		float m_MouseSensitivity;

		// For projection
		float m_FOV;
		float m_AspectRatio;
		float m_NearClip;
		float m_FarClip;
	};

} // namespace Engine