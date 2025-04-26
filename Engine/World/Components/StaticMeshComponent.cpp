/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticMeshComponent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:28:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:50:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/StaticMeshComponent.h"
#include "Renderer/MaterialPBR.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h" // Include Texture header for Bind method
#include "World/Actor.h"
#include "World/Components/SceneComponent.h"

namespace Engine {

	StaticMeshComponent::StaticMeshComponent(Actor *owner, Mesh *mesh)
		: ActorComponent(owner), m_Mesh(mesh), m_Model(nullptr), m_Material(nullptr) {
		// Directly assign the raw pointer. Lifetime is managed externally (e.g., in Application).
		// No need for reset/release.
	}

	StaticMeshComponent::StaticMeshComponent(Actor *owner, const std::string &objPath, std::shared_ptr<MaterialPBR> material)
		: ActorComponent(owner), m_Mesh(nullptr), m_Model(std::make_unique<Model>(objPath)), m_Material(material) {
	}

	StaticMeshComponent::~StaticMeshComponent() = default;

	void StaticMeshComponent::Render(Shader &shader) {
		// Get world transform from owner's root component
		glm::mat4 modelMatrix = GetOwner()->GetRootComponent()->GetWorldTransform();
		shader.SetUniformMat4("u_Model", modelMatrix); // Match vertex shader uniform name

		// --- Material Handling ---
		bool usedMaterial = false;
		if (m_Material) {
			// --- Texture Binding & Uniforms ---
			// Bind Albedo (Unit 0)
			if (m_Material->hasAlbedoMap && m_Material->albedoMap) {
				m_Material->albedoMap->Bind(0);
				shader.SetUniformInt("u_AlbedoMap", 0); // Unit 0
				shader.SetUniformInt("u_HasAlbedoMap", 1);
			} else {
				shader.SetUniformInt("u_HasAlbedoMap", 0);
				shader.SetUniformVec3("u_AlbedoColor", m_Material->albedoColor); // Use fallback color
			}

			// Bind Normal (Unit 1)
			if (m_Material->hasNormalMap && m_Material->normalMap) {
				m_Material->normalMap->Bind(1);
				shader.SetUniformInt("u_NormalMap", 1); // Unit 1
				shader.SetUniformInt("u_HasNormalMap", 1);
			} else {
				shader.SetUniformInt("u_HasNormalMap", 0);
			}

			// Bind AO (Separate Map - Unit 3)
			// Note: ORM map (Unit 2) is not handled here as MaterialPBR only has separate AO.
			shader.SetUniformInt("u_HasORMMap", 0); // Explicitly state no ORM map from this component
			if (m_Material->hasAOMap && m_Material->aoMap) {
				m_Material->aoMap->Bind(3);			// Bind separate AO to Unit 3
				shader.SetUniformInt("u_AOMap", 3); // Tell shader to use texture unit 3
				shader.SetUniformInt("u_HasAOMap", 1);
			} else {
				shader.SetUniformInt("u_HasAOMap", 0);
				shader.SetUniformFloat("u_AO", m_Material->ao); // Use fallback AO value if no map
			}

			// --- Float Uniforms ---
			shader.SetUniformFloat("u_Metallic", m_Material->metallic);
			shader.SetUniformFloat("u_Roughness", m_Material->roughness);
			// Only set fallback AO if there's no AO map (handled above)
			if (!m_Material->hasAOMap) {
				shader.SetUniformFloat("u_AO", m_Material->ao);
			}

			usedMaterial = true;
		}
		// --- End Material Handling ---

		// --- Draw Call ---
		if (m_Model) {
			// If we didn't use an explicit PBR material, set default PBR properties
			if (!usedMaterial) {
				// Set default PBR values if using PBR shader
				shader.SetUniformInt("u_HasAlbedoMap", 0);					// No texture
				shader.SetUniformVec3("u_AlbedoColor", {0.8f, 0.8f, 0.8f}); // Default to grey
				shader.SetUniformInt("u_HasNormalMap", 0);					// No normal map
				shader.SetUniformInt("u_HasORMMap", 0);						// No ORM map
				shader.SetUniformInt("u_HasAOMap", 0);						// No separate AO map
				shader.SetUniformFloat("u_Metallic", 0.1f);					// Default model: slightly metallic
				shader.SetUniformFloat("u_Roughness", 0.8f);				// Default model: quite rough
				shader.SetUniformFloat("u_AO", 1.0f);						// Default model: no AO
			}
			// Draw the model geometry (uniforms are set either by material or defaults)
			m_Model->DrawGeometry(shader);

		} else if (m_Mesh) {
			// If drawing a primitive mesh and didn't use an explicit PBR material, set default PBR material properties
			if (!usedMaterial) {
				// Set default PBR values if using PBR shader
				shader.SetUniformInt("u_HasAlbedoMap", 0);					// No texture for basic primitives
				shader.SetUniformVec3("u_AlbedoColor", {1.0f, 1.0f, 1.0f}); // Default to white
				shader.SetUniformInt("u_HasNormalMap", 0);					// No normal map
				shader.SetUniformInt("u_HasORMMap", 0);						// No ORM map
				shader.SetUniformInt("u_HasAOMap", 0);						// No separate AO map
				shader.SetUniformFloat("u_Metallic", 0.0f);					// Default primitive: non-metallic
				shader.SetUniformFloat("u_Roughness", 0.5f);				// Default primitive: mid-roughness
				shader.SetUniformFloat("u_AO", 1.0f);						// Default primitive: no AO
			}
			// Draw the primitive mesh geometry
			m_Mesh->Draw(); // Mesh::Draw binds VAO and calls glDrawElements. Shader is already bound.
		}
	}

	void StaticMeshComponent::SetMaterial(std::shared_ptr<MaterialPBR> material) {
		m_Material = material;
	}

} // namespace Engine