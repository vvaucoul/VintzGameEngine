/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BillboardComponent.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:40:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:27:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/BillboardComponent.h"
#include "Renderer/Primitives/Primitives.h" // For CreateQuad
#include "Renderer/Shader.h"
#include "World/Actor.h" // Although owner is passed, Actor definition might be needed indirectly
#include <glad/glad.h>	 // Include GLAD for OpenGL functions
#include <glm/gtc/matrix_transform.hpp>
#include <iostream> // For error messages

namespace Engine {

	// Initialize static member
	std::unique_ptr<Mesh> BillboardComponent::s_QuadMesh = nullptr;

	void BillboardComponent::InitQuad() {
		if (!s_QuadMesh) {
			s_QuadMesh = Primitives::CreateQuad();
			if (!s_QuadMesh) {
				std::cerr << "[ERROR] BillboardComponent: Failed to create quad mesh!" << std::endl;
			}
		}
	}

	BillboardComponent::BillboardComponent(Actor *owner, const std::string &spritePath, const glm::vec2 &size)
		: SceneComponent(owner), m_Size(size) {
		InitQuad(); // Ensure the quad mesh is ready
		if (!spritePath.empty()) {
			SetSprite(spritePath);
		}
	}

	void BillboardComponent::SetSprite(std::shared_ptr<Texture> texture) {
		m_SpriteTexture = texture;
	}

	void BillboardComponent::SetSprite(const std::string &path) {
		try {
			m_SpriteTexture = std::make_shared<Texture>(path);
		} catch (const std::exception &e) {
			std::cerr << "[ERROR] BillboardComponent: Failed to load texture '" << path << "': " << e.what() << std::endl;
			m_SpriteTexture = nullptr;
		} catch (...) {
			std::cerr << "[ERROR] BillboardComponent: Failed to load texture '" << path << "' (Unknown error)." << std::endl;
			m_SpriteTexture = nullptr;
		}
	}

	void BillboardComponent::SetSize(const glm::vec2 &size) {
		m_Size = size;
	}

	void BillboardComponent::Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode) {
		// Note: RenderMode 'mode' is currently ignored here. Billboards are always
		// rendered textured/unlit. Wireframe mode skips them in World::Render.
		if (!m_SpriteTexture || !s_QuadMesh) {
			return; // Cannot render without texture or mesh
		}

		// --- OpenGL State Setup for Billboards (Transparency) ---
		// DepthMask and BlendFunc are set here, assuming billboards are rendered last.
		// If render order changes, this might need adjustment.
		glDepthMask(GL_FALSE);
		// Blending should already be enabled globally in Application::Init
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 1. Get Billboard World Position
		glm::vec3 worldPosition = GetWorldTransform()[3]; // Extract translation from world matrix

		// 2. Get Camera Orientation Vectors from View Matrix
		// View matrix inverse transpose gives camera orientation in world space
		// Alternatively, extract directly from view matrix (transposed rotation part)
		glm::vec3 cameraRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		glm::vec3 cameraUp	  = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
		// glm::vec3 cameraForward = glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]); // Points out of screen

		// 3. Calculate Model Matrix
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix			  = glm::translate(modelMatrix, worldPosition);
		// Construct rotation part to face camera
		// Billboard's local +X aligns with Camera's world +Right
		// Billboard's local +Y aligns with Camera's world +Up
		// Billboard's local +Z aligns with Camera's world -Forward (towards camera)
		modelMatrix[0] = glm::vec4(cameraRight, 0.0f);
		modelMatrix[1] = glm::vec4(cameraUp, 0.0f);
		modelMatrix[2] = glm::vec4(-glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]), 0.0f); // -Forward
		// Apply scaling
		modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Size.x, m_Size.y, 1.0f));

		// 4. Set Shader Uniforms
		shader.Bind(); // Ensure shader is bound
		shader.SetUniformMat4("u_Model", modelMatrix);

		// Set uniforms appropriate for the shader being used (PBR or Unlit)
		if (mode == RenderMode::Default) { // PBR
			shader.SetUniformInt("u_AlbedoMap", 0);
			shader.SetUniformInt("u_HasAlbedoMap", 1);
			shader.SetUniformInt("u_HasNormalMap", 0);
			shader.SetUniformInt("u_HasMetallicMap", 0);
			shader.SetUniformInt("u_HasRoughnessMap", 0);
			shader.SetUniformInt("u_HasAOMap", 0);
			shader.SetUniformVec3("u_AlbedoColor", {1.0f, 1.0f, 1.0f});
			shader.SetUniformFloat("u_Metallic", 0.0f);
			shader.SetUniformFloat("u_Roughness", 1.0f);
			shader.SetUniformFloat("u_AO", 1.0f);
		} else { // Unlit
			shader.SetUniformInt("u_AlbedoMap", 0);
			shader.SetUniformInt("u_HasAlbedoMap", 1);
			shader.SetUniformVec3("u_AlbedoColor", {1.0f, 1.0f, 1.0f});
		}

		// 5. Bind Texture
		m_SpriteTexture->Bind(0);

		// 6. Draw Quad
		s_QuadMesh->Draw();

		// --- Restore OpenGL State ---
		glDepthMask(GL_TRUE);
		// Blending remains enabled
		// glDisable(GL_BLEND);
	}

} // namespace Engine
