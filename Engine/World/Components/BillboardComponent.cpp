/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BillboardComponent.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:40:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:55:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "World/Components/BillboardComponent.h"
#include "Renderer/Primitives/Primitives.h"
#include "Renderer/Shaders/Shader.h"
#include "World/Actor.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Engine {

	std::unique_ptr<Mesh> BillboardComponent::s_QuadMesh = nullptr;

	void BillboardComponent::InitQuad() {
		if (!s_QuadMesh) {
			s_QuadMesh = Primitives::CreateQuad();
			if (!s_QuadMesh) {
				std::cerr << "[BillboardComponent] Failed to create quad mesh." << std::endl;
			}
		}
	}

	BillboardComponent::BillboardComponent(Actor *owner, const std::string &spritePath, const glm::vec2 &size)
		: SceneComponent(owner), m_Size(size) {
		InitQuad();
		if (!spritePath.empty()) {
			SetSprite(spritePath);
		}
	}

	void BillboardComponent::SetSprite(std::shared_ptr<Texture> texture) {
		m_SpriteTexture = std::move(texture);
	}

	void BillboardComponent::SetSprite(const std::string &path) {
		try {
			m_SpriteTexture = std::make_shared<Texture>(path);
		} catch (const std::exception &e) {
			std::cerr << "[BillboardComponent] Texture load failed: " << path << " (" << e.what() << ")" << std::endl;
			m_SpriteTexture = nullptr;
		} catch (...) {
			std::cerr << "[BillboardComponent] Texture load failed: " << path << " (unknown error)" << std::endl;
			m_SpriteTexture = nullptr;
		}
	}

	void BillboardComponent::SetSize(const glm::vec2 &size) {
		m_Size = size;
	}

	void BillboardComponent::Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode) {
		if (!m_SpriteTexture || !s_QuadMesh) {
			return;
		}

		// Enable transparency for billboards (assume blending enabled globally)
		glDepthMask(GL_FALSE);

		// Extract world position from transform
		glm::vec3 worldPos = glm::vec3(GetWorldTransform()[3]);

		// Extract camera orientation from view matrix (columns are camera axes)
		glm::vec3 camRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		glm::vec3 camUp	   = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
		glm::vec3 camFwd   = glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);

		// Build model matrix: translate, orient to camera, scale
		glm::mat4 model = glm::mat4(1.0f);
		model			= glm::translate(model, worldPos);
		model[0]		= glm::vec4(camRight, 0.0f);
		model[1]		= glm::vec4(camUp, 0.0f);
		model[2]		= glm::vec4(-camFwd, 0.0f);
		model			= glm::scale(model, glm::vec3(m_Size, 1.0f));

		shader.Bind();
		shader.SetUniformMat4("u_Model", model);

		// Set shader uniforms (PBR or Unlit)
		if (mode == RenderMode::Default) {
			shader.SetUniformInt("u_AlbedoMap", 0);
			shader.SetUniformInt("u_HasAlbedoMap", 1);
			shader.SetUniformInt("u_HasNormalMap", 0);
			shader.SetUniformInt("u_HasMetallicMap", 0);
			shader.SetUniformInt("u_HasRoughnessMap", 0);
			shader.SetUniformInt("u_HasAOMap", 0);
			shader.SetUniformVec3("u_AlbedoColor", glm::vec3(1.0f));
			shader.SetUniformFloat("u_Metallic", 0.0f);
			shader.SetUniformFloat("u_Roughness", 1.0f);
			shader.SetUniformFloat("u_AO", 1.0f);
		} else {
			shader.SetUniformInt("u_AlbedoMap", 0);
			shader.SetUniformInt("u_HasAlbedoMap", 1);
			shader.SetUniformVec3("u_AlbedoColor", glm::vec3(1.0f));
		}

		m_SpriteTexture->Bind(0);
		s_QuadMesh->Draw();

		glDepthMask(GL_TRUE);
	}

} // namespace Engine
