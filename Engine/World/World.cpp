/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:24:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:50:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/World.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "World/Actor.h"
#include "World/Components/DirectionalLightComponent.h" // Include Directional Light
#include "World/Components/PointLightComponent.h"		// Include Point Light
#include "World/Components/StaticMeshComponent.h"
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

	void World::Render(Shader &shader, [[maybe_unused]] Camera &camera) { // Mark camera unused for now
		shader.Bind();													  // Ensure shader is bound before setting uniforms

		// Apply all DirectionalLightComponents
		for (auto &actor : m_Actors) {
			auto dirLight = actor->GetComponent<DirectionalLightComponent>();
			if (dirLight) {
				dirLight->Apply(shader);
				// Optimization: Assume only one directional light for now
				// break;
			}
		}

		// Collect and apply all PointLightComponents
		std::vector<PointLightComponent *> pointLights;
		for (auto &actor : m_Actors) {
			auto pointLight = actor->GetComponent<PointLightComponent>();
			if (pointLight) {
				pointLights.push_back(pointLight);
			}
		}
		// Limit the number of point lights sent to the shader if needed
		// const int MAX_POINT_LIGHTS = 4; // Example limit
		// int numLightsToSend = std::min((int)pointLights.size(), MAX_POINT_LIGHTS);
		int numLightsToSend = static_cast<int>(pointLights.size());
		shader.SetUniformInt("numPointLights", numLightsToSend);
		for (int i = 0; i < numLightsToSend; ++i) {
			pointLights[i]->Apply(shader, i);
		}

		// Assume UBO for viewProjection is already set up externally
		// Set view position uniform (needed for specular lighting)
		// shader.SetUniformVec3("u_ViewPos", camera.GetPosition()); // Requires GetPosition() in Camera

		for (auto &actor : m_Actors) {
			// Render only if actor has a StaticMeshComponent
			auto sm = actor->GetComponent<StaticMeshComponent>();
			if (sm)
				sm->Render(shader); // Shader is already bound here
		}
		// Shader should be unbound after all rendering using it is done,
		// likely outside this function in Application::MainLoop
	}

} // namespace Engine