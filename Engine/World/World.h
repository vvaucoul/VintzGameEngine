/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:15:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 23:27:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
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

		// Render all actors that have StaticMeshComponent
		void Render(Shader &shader);

		// Render depth for shadow mapping
		void RenderDepth(Shader &depthShader);

		// Getters
		const std::vector<std::unique_ptr<Actor>> &GetActors() const;

	private:
		std::vector<std::unique_ptr<Actor>> m_Actors;
		uint32_t m_NextID;
	};

} // namespace Engine