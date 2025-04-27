/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:24:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 23:27:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/World.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"
#include "World/Components/DirectionalLightComponent.h"
#include "World/Components/PointLightComponent.h"
#include "World/Components/SceneComponent.h"
#include "World/Components/SpotLightComponent.h"
#include "World/Components/StaticMeshComponent.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace Engine {

	World::World()
		: m_NextID(1) {
	}

	World::~World() = default;

	/**
	 * @brief Spawns a new Actor in the world.
	 * @return Reference to the newly created Actor.
	 */
	Actor &World::SpawnActor() {
		auto actor = std::make_unique<Actor>(m_NextID++, this);
		Actor &ref = *actor;
		m_Actors.push_back(std::move(actor));
		return ref;
	}

	/**
	 * @brief Updates all actors in the world.
	 * @param deltaTime Time elapsed since last frame (in seconds).
	 */
	void World::Tick(float deltaTime) {
		for (auto &actor : m_Actors) {
			actor->Tick(deltaTime);
		}
	}

	/**
	 * @brief Renders all visible actors and sets up lighting uniforms.
	 * @param shader The shader used for rendering meshes and lighting.
	 *
	 * Iterates over all actors to:
	 *   - Setup uniforms for all light components (directional, point, spot).
	 *   - Render all StaticMeshComponents.
	 * The number of active lights is passed to the shader for correct array indexing.
	 */
	void World::Render(Shader &shader) {
		// --- Light Setup ---
		int pointLightCount		   = 0;
		int spotLightCount		   = 0;
		const int MAX_POINT_LIGHTS = 4; // Must match shader definition
		const int MAX_SPOT_LIGHTS  = 4; // Must match shader definition

		// Iterate through all actors to find and setup light components
		for (const auto &actor : m_Actors) {
			// Directional Light (only one supported for now)
			auto dirLight = actor->GetComponent<DirectionalLightComponent>();
			if (dirLight) {
				dirLight->SetupUniforms(shader, 0); // Index 0 for single directional light
													// Only one directional light is supported; break if needed
			}

			// Point Lights
			auto pointLight = actor->GetComponent<PointLightComponent>();
			if (pointLight && pointLightCount < MAX_POINT_LIGHTS) {
				pointLight->SetupUniforms(shader, pointLightCount);
				pointLightCount++;
			}

			// Spot Lights
			auto spotLight = actor->GetComponent<SpotLightComponent>();
			if (spotLight && spotLightCount < MAX_SPOT_LIGHTS) {
				spotLight->SetupUniforms(shader, spotLightCount);
				spotLightCount++;
			}
		}

		// Pass the number of active lights to the shader
		shader.SetUniformInt("numPointLights", pointLightCount);
		shader.SetUniformInt("numSpotLights", spotLightCount);

		// --- Render Static Meshes ---
		// Render all StaticMeshComponents in the world
		for (const auto &actor : m_Actors) {
			auto meshComp = actor->GetComponent<StaticMeshComponent>();
			if (meshComp) {
				meshComp->Render(shader);
			}
		}
	}

	/**
	 * @brief Renders depth information for all static meshes (for shadow mapping).
	 * @param depthShader The shader used for depth rendering.
	 *
	 * Sets the model matrix uniform for each mesh before rendering.
	 */
	void World::RenderDepth(Shader &depthShader) {
		for (auto &actor : m_Actors) {
			auto meshComp = actor->GetComponent<StaticMeshComponent>();
			if (meshComp) {
				// Set the model matrix for the current actor
				depthShader.SetUniformMat4("model", actor->GetRootComponent()->GetWorldTransform());
				meshComp->RenderDepth(depthShader);
			}
		}
	}

	/**
	 * @brief Returns a const reference to the list of actors in the world.
	 */
	const std::vector<std::unique_ptr<Actor>> &World::GetActors() const {
		return m_Actors;
	}

} // namespace Engine