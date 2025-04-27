/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:24:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:53:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/World.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"
#include "World/Components/DirectionalLightComponent.h" // Include Directional Light
#include "World/Components/PointLightComponent.h"		// Include Point Light
#include "World/Components/SceneComponent.h"			// Add this include
#include "World/Components/SpotLightComponent.h"
#include "World/Components/StaticMeshComponent.h"
#include <algorithm> // For std::find_if
#include <iostream>
#include <vector> // Ensure vector is included

namespace Engine {

	World::World()
		: m_NextID(1) {
	}

	World::~World() = default;

	Actor &World::SpawnActor() {
		auto actor = std::make_unique<Actor>(m_NextID++, this);
		Actor &ref = *actor;
		m_Actors.push_back(std::move(actor));
		return ref;
	}

	void World::Tick(float deltaTime) {
		for (auto &actor : m_Actors) {
			actor->Tick(deltaTime);
		}
	}

	void World::Render(Shader &shader) { // Remove const Camera& camera parameter
		// --- Light Setup ---
		int pointLightCount = 0;
		int spotLightCount	= 0;
		// Define maximums (should match shader)
		const int MAX_POINT_LIGHTS = 4;
		const int MAX_SPOT_LIGHTS  = 4; // Ensure this matches shader define

		// Iterate through all actors to find light components
		for (const auto &actor : m_Actors) {
			// Check for Directional Light (only one supported for now)
			auto dirLight = actor->GetComponent<DirectionalLightComponent>();
			if (dirLight) {
				dirLight->SetupUniforms(shader, 0); // Index 0 is arbitrary here
													// Found the directional light, potentially break if only one is expected
			}

			// Check for Point Lights
			auto pointLight = actor->GetComponent<PointLightComponent>();
			if (pointLight && pointLightCount < MAX_POINT_LIGHTS) {
				pointLight->SetupUniforms(shader, pointLightCount);
				pointLightCount++;
			}

			// Check for Spot Lights
			auto spotLight = actor->GetComponent<SpotLightComponent>();
			if (spotLight && spotLightCount < MAX_SPOT_LIGHTS) {
				spotLight->SetupUniforms(shader, spotLightCount);
				spotLightCount++;
			}
		}

		// Set the number of active lights in the shader
		shader.SetUniformInt("numPointLights", pointLightCount);
		shader.SetUniformInt("numSpotLights", spotLightCount); // Uncomment this line

		// --- Render Static Meshes ---
		// Iterate through actors and render their StaticMeshComponents
		for (const auto &actor : m_Actors) {
			auto meshComp = actor->GetComponent<StaticMeshComponent>();
			if (meshComp) {
				meshComp->Render(shader);
			}
		}
	}

	void World::RenderDepth(Shader &depthShader) {
		for (auto &actor : m_Actors) {
			// Find StaticMeshComponent within the actor
			auto meshComp = actor->GetComponent<StaticMeshComponent>();
			if (meshComp) {
				// Set the model matrix uniform for the depth shader
				depthShader.SetUniformMat4("model", actor->GetRootComponent()->GetWorldTransform()); // This line should now compile
				meshComp->RenderDepth(depthShader);
			}
		}
	}

	const std::vector<std::unique_ptr<Actor>> &World::GetActors() const {
		return m_Actors;
	}

} // namespace Engine