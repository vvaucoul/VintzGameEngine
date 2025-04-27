/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticMeshComponent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:28:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 23:01:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/StaticMeshComponent.h"
#include "Renderer/MaterialPBR.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "World/Actor.h"
#include "World/Components/SceneComponent.h"

namespace Engine {

	// --- Constructors & Destructor ---

	StaticMeshComponent::StaticMeshComponent(Actor *owner, Mesh *mesh)
		: ActorComponent(owner), m_Mesh(mesh), m_Model(nullptr), m_Material(nullptr) {
		// Mesh pointer is managed externally (e.g., by Application)
	}

	StaticMeshComponent::StaticMeshComponent(Actor *owner, const std::string &objPath, std::shared_ptr<MaterialPBR> material)
		: ActorComponent(owner), m_Mesh(nullptr), m_Model(std::make_unique<Model>(objPath)), m_Material(material) {}

	StaticMeshComponent::~StaticMeshComponent() = default;

	// --- Rendering Methods ---

	void StaticMeshComponent::Render(Shader &shader) {
		// Get world transform from owner's root SceneComponent
		const glm::mat4 modelMatrix = GetOwner()->GetRootComponent()->GetWorldTransform();
		shader.SetUniformMat4("u_Model", modelMatrix);

		// --- Geometry Draw Call ---
		if (m_Model) {
			// If a model exists, delegate drawing and material handling to it.
			// Model::Draw will handle setting its own material uniforms internally.
			m_Model->Draw(shader);
		} else if (m_Mesh) {
			// Handle primitive meshes with optional explicitly set material
			bool hasMaterial = (m_Material != nullptr);

			// --- Material & Texture Uniforms for Primitive Mesh ---
			if (hasMaterial) {
				// Albedo
				if (m_Material->hasAlbedoMap && m_Material->albedoMap) {
					m_Material->albedoMap->Bind(0);
					shader.SetUniformInt("u_AlbedoMap", 0);
					shader.SetUniformInt("u_HasAlbedoMap", 1);
				} else {
					shader.SetUniformInt("u_HasAlbedoMap", 0);
					shader.SetUniformVec3("u_AlbedoColor", m_Material->albedoColor);
				}

				// Normal
				if (m_Material->hasNormalMap && m_Material->normalMap) {
					m_Material->normalMap->Bind(1);
					shader.SetUniformInt("u_NormalMap", 1);
					shader.SetUniformInt("u_HasNormalMap", 1);
				} else {
					shader.SetUniformInt("u_HasNormalMap", 0);
				}

				// Ambient Occlusion (AO)
				if (m_Material->hasAOMap && m_Material->aoMap) {
					m_Material->aoMap->Bind(3);
					shader.SetUniformInt("u_AOMap", 3);
					shader.SetUniformInt("u_HasAOMap", 1);
				} else {
					shader.SetUniformInt("u_HasAOMap", 0);
					// Only set AO factor if no map
					shader.SetUniformFloat("u_AO", m_Material->ao);
				}

				// PBR floats
				shader.SetUniformFloat("u_Metallic", m_Material->metallic);
				shader.SetUniformFloat("u_Roughness", m_Material->roughness);
				// Set AO factor if no map (redundant check removed, handled above)
				// if (!m_Material->hasAOMap)
				//	shader.SetUniformFloat("u_AO", m_Material->ao);
			} else {
				// Set default PBR values for primitives if no material
				shader.SetUniformInt("u_HasAlbedoMap", 0);
				shader.SetUniformVec3("u_AlbedoColor", {1.0f, 1.0f, 1.0f}); // Default white for primitives
				shader.SetUniformInt("u_HasNormalMap", 0);
				shader.SetUniformInt("u_HasAOMap", 0);
				shader.SetUniformFloat("u_Metallic", 0.0f);	 // Non-metallic default
				shader.SetUniformFloat("u_Roughness", 0.5f); // Semi-rough default
				shader.SetUniformFloat("u_AO", 1.0f);		 // Full ambient occlusion default
			}
			m_Mesh->Draw();
		}
	}

	void StaticMeshComponent::RenderDepth(Shader &depthShader) {
		// Set model matrix for depth shader
		const glm::mat4 modelMatrix = GetOwner()->GetRootComponent()->GetWorldTransform();
		depthShader.SetUniformMat4("model", modelMatrix);

		// Draw geometry (no material needed)
		if (m_Model) {
			m_Model->DrawGeometry(depthShader);
		} else if (m_Mesh) {
			m_Mesh->Draw();
		}
	}

	// --- Material Setter ---

	void StaticMeshComponent::SetMaterial(std::shared_ptr<MaterialPBR> material) {
		m_Material = material;
	}

} // namespace Engine
