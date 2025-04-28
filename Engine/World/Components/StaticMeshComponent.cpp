/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticMeshComponent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:28:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:58:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/StaticMeshComponent.h"
#include "Renderer/Geometry/Mesh.h"
#include "Renderer/Geometry/Model.h"
#include "Renderer/Materials/DefaultMaterial.h" // Include default material getter
#include "Renderer/Materials/MaterialPBR.h"
#include "Renderer/Shaders/Shader.h"
#include "Renderer/Textures/Texture.h"
#include "World/Actor.h"
#include "World/Components/SceneComponent.h"

namespace Engine {

	// --- Constructors & Destructor ---

	StaticMeshComponent::StaticMeshComponent(Actor *owner, Mesh *mesh)
		: ActorComponent(owner), m_Mesh(mesh), m_Model(nullptr), m_Material(GetDefaultMaterial()) {
		// Mesh pointer is managed externally. Always initialize with default material.
	}

	StaticMeshComponent::StaticMeshComponent(Actor *owner, const std::string &objPath, std::shared_ptr<MaterialPBR> material)
		: ActorComponent(owner), m_Mesh(nullptr), m_Model(std::make_unique<Model>(objPath)), m_Material(material ? material : GetDefaultMaterial()) {
		// Use provided material or default if null.
		// If the model loaded its own materials, they should ideally override this default.
		// Consider adding logic here or in Model::Draw to prioritize Model's materials if they exist.
		// For now, this component's material takes precedence if set explicitly after construction.
		// If the model has materials, we might want to use those instead. Let's refine this.
		// If the model loads materials, use the first one as default for the component?
		// Or let Model::Draw handle its own materials entirely? Let's assume Model::Draw handles its own.
		// This m_Material will be used primarily if m_Model is null OR if we want to override the model's material.
		// Let's stick to the current logic: use provided or default. Model::Draw will handle its internal materials.
	}

	StaticMeshComponent::~StaticMeshComponent() = default;

	// --- Rendering Methods ---

	void StaticMeshComponent::Render(Shader &shader, RenderMode mode) {
		// Get world transform from owner's root SceneComponent
		const glm::mat4 modelMatrix = GetOwner()->GetRootComponent()->GetWorldTransform();
		shader.SetUniformMat4("u_Model", modelMatrix);

		// --- Geometry Draw Call ---
		if (m_Model) {
			// Wireframe mode only needs geometry
			if (mode == RenderMode::Wireframe) {
				m_Model->DrawGeometry(shader);
			} else {
				// PBR and Unlit modes use materials (Model::Draw handles its internal materials)
				m_Model->Draw(shader);
			}
		} else if (m_Mesh) {
			// --- Material & Texture Uniforms for Primitive Mesh ---
			if (mode == RenderMode::Default) { // PBR
				// --- PBR Uniforms ---
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
				// Metallic
				if (m_Material->hasMetallicMap && m_Material->metallicMap) {
					m_Material->metallicMap->Bind(2);
					shader.SetUniformInt("u_MetallicMap", 2);
					shader.SetUniformInt("u_HasMetallicMap", 1);
				} else {
					shader.SetUniformInt("u_HasMetallicMap", 0);
					shader.SetUniformFloat("u_Metallic", m_Material->metallic);
				}
				// Roughness
				if (m_Material->hasRoughnessMap && m_Material->roughnessMap) {
					m_Material->roughnessMap->Bind(3);
					shader.SetUniformInt("u_RoughnessMap", 3);
					shader.SetUniformInt("u_HasRoughnessMap", 1);
				} else {
					shader.SetUniformInt("u_HasRoughnessMap", 0);
					shader.SetUniformFloat("u_Roughness", m_Material->roughness);
				}
				// AO
				if (m_Material->hasAOMap && m_Material->aoMap) {
					m_Material->aoMap->Bind(5); // Use texture unit 5 for AO (Shadow map uses 4)
					shader.SetUniformInt("u_AOMap", 5);
					shader.SetUniformInt("u_HasAOMap", 1);
				} else {
					shader.SetUniformInt("u_HasAOMap", 0);
					shader.SetUniformFloat("u_AO", m_Material->ao);
				}

			} else if (mode == RenderMode::Unlit) {
				// --- Unlit Uniforms ---
				if (m_Material->hasAlbedoMap && m_Material->albedoMap) {
					m_Material->albedoMap->Bind(0);
					shader.SetUniformInt("u_AlbedoMap", 0);
					shader.SetUniformInt("u_HasAlbedoMap", 1);
				} else {
					shader.SetUniformInt("u_HasAlbedoMap", 0);
					shader.SetUniformVec3("u_AlbedoColor", m_Material->albedoColor);
				}
			} else if (mode == RenderMode::Wireframe) {
				// --- Wireframe Uniforms ---
				shader.SetUniformVec4("u_WireColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// Draw the mesh (common to all modes)
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

	void StaticMeshComponent::RenderGeometry(Shader &shader) {
		SceneComponent *sceneComp = GetOwner()->GetRootComponent();
		if (!sceneComp) return;

		shader.SetUniformMat4("u_Model", sceneComp->GetWorldTransform());

		// Envoi des uniforms pour le GBuffer
		shader.SetUniformVec3("u_Material_AlbedoColor", m_Material->albedoColor);
		shader.SetUniformFloat("u_Material_Metallic", m_Material->metallic);
		shader.SetUniformFloat("u_Material_Roughness", m_Material->roughness);
		shader.SetUniformFloat("u_Material_AO", m_Material->ao);

		// Pour l’instant, on suppose pas de textures (sinon il faut les binder et set les booléens)
		shader.SetUniformInt("u_Material_HasAlbedoMap", 0);
		shader.SetUniformInt("u_Material_HasMetallicMap", 0);
		shader.SetUniformInt("u_Material_HasRoughnessMap", 0);
		shader.SetUniformInt("u_Material_HasAOMap", 0);

		if (m_Model) {
			m_Model->DrawGeometry(shader);
		} else if (m_Mesh) {
			m_Mesh->Draw();
		}
	}

	// --- Material Setter ---

	void StaticMeshComponent::SetMaterial(std::shared_ptr<MaterialPBR> material) {
		m_Material = material;
	}

} // namespace Engine
