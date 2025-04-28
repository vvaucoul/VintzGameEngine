/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:15:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:27:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Core/Application.h" // Include Application for RenderMode enum
#include <cstdint>
#include <glm/mat4x4.hpp> // Include for glm::mat4
#include <memory>
#include <vector>

namespace Engine {

	class Actor;
	class Shader;
	class Camera;

	class World {
	public:
		World();
		~World();

		// Spawn a new Actor in the world
		Actor &SpawnActor();

		// Called every frame
		void Tick(float deltaTime);

		/**
		 * @brief Renders all visible actors and sets up lighting uniforms based on mode.
		 * @param shader The shader selected based on the render mode.
		 * @param viewMatrix The current camera view matrix.
		 * @param mode The current rendering mode (Default, Unlit, Wireframe).
		 */
		void Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode);

		// Render depth for shadow mapping
		void RenderDepth(Shader &depthShader);

		// Getters
		const std::vector<std::unique_ptr<Actor>> &GetActors() const;

	private:
		std::vector<std::unique_ptr<Actor>> m_Actors;
		uint32_t m_NextID;
	};

} // namespace Engine