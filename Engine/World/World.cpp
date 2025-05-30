/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:24:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 10:56:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/World.h"
#include "Core/Application.h" // Include Application to check render mode (or pass shader pointer type)
#include "Renderer/Camera.h"
#include "Renderer/Shaders/Shader.h"
#include "World/Actor.h"
#include "World/Components/BillboardComponent.h" // Include BillboardComponent
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
	 * @brief Renders all visible actors and sets up lighting uniforms based on mode.
	 * @param shader The shader selected based on the render mode.
	 * @param viewMatrix The current camera view matrix.
	 * @param mode The current rendering mode (Default, Unlit, Wireframe).
	 */
	void World::Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode) {
		// --- Light Setup (Only for PBR/Default Mode) ---
		if (mode == RenderMode::Default) {
			int pointLightCount		   = 0;
			int spotLightCount		   = 0;
			const int MAX_POINT_LIGHTS = 4;
			const int MAX_SPOT_LIGHTS  = 4;

			for (const auto &actor : m_Actors) {
				// Directional Light
				if (auto dirLight = actor->GetComponent<DirectionalLightComponent>()) {
					dirLight->SetupUniforms(shader, 0);
				}
				// Point Lights (excluding SpotLights)
				if (auto pointLight = actor->GetComponent<PointLightComponent>()) {
					if (!dynamic_cast<SpotLightComponent *>(pointLight) && pointLightCount < MAX_POINT_LIGHTS) {
						pointLight->SetupUniforms(shader, pointLightCount++);
					}
				}
				// Spot Lights
				if (auto spotLight = actor->GetComponent<SpotLightComponent>()) {
					if (spotLightCount < MAX_SPOT_LIGHTS) {
						spotLight->SetupUniforms(shader, spotLightCount++);
					}
				}
			}
			shader.SetUniformInt("u_NumPointLights", pointLightCount);
			shader.SetUniformInt("u_NumSpotLights", spotLightCount);
			shader.SetUniformInt("u_HasDirLight", (int)(pointLightCount > 0));
			shader.SetUniformInt("u_HasPointLight", (int)(pointLightCount > 0));
			shader.SetUniformInt("u_HasSpotLight", (int)(spotLightCount > 0));
		}

		// --- Render Static Meshes & Collect Billboards ---
		std::vector<BillboardComponent *> billboards;
		for (const auto &actor : m_Actors) {
			// Render Meshes
			auto meshComp = actor->GetComponent<StaticMeshComponent>();
			if (meshComp) {
				meshComp->Render(shader, mode); // Pass shader and mode
			}
			// Collect Billboards
			auto actorBillboards = actor->GetComponentsByClass<BillboardComponent>();
			billboards.insert(billboards.end(), actorBillboards.begin(), actorBillboards.end());
		}

		// --- Render Billboards ---
		// Don't render billboards in wireframe mode
		if (mode != RenderMode::Wireframe) {
			for (BillboardComponent *billboard : billboards) {
				if (billboard) {
					billboard->Render(shader, viewMatrix, mode); // Pass shader, view matrix, and mode
				}
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