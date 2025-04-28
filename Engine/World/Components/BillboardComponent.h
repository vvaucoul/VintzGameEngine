/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BillboardComponent.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:38:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:04:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Core/Application.h"
#include "Renderer/Mesh.h"
#include "Renderer/Textures/Texture.h"
#include "World/Components/SceneComponent.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Engine {

	class Shader;
	class Actor;

	/**
	 * @brief Component that renders a camera-facing textured quad ("billboard").
	 *
	 * BillboardComponent is a SceneComponent that displays a 2D sprite in 3D space,
	 * always oriented to face the camera. Useful for sprites, particles, icons, etc.
	 */
	class BillboardComponent : public SceneComponent {
	public:
		/**
		 * @brief Construct a BillboardComponent.
		 * @param owner Owning Actor.
		 * @param spritePath Optional path to the sprite texture.
		 * @param size Billboard size in world units (width, height).
		 */
		BillboardComponent(Actor *owner, const std::string &spritePath = "", const glm::vec2 &size = glm::vec2(1.0f, 1.0f));

		/**
		 * @brief Destructor.
		 */
		~BillboardComponent() override = default;

		/**
		 * @brief Set the sprite texture from a Texture object.
		 * @param texture Shared pointer to the texture.
		 */
		void SetSprite(std::shared_ptr<Texture> texture);

		/**
		 * @brief Load and set the sprite texture from a file path.
		 * @param path Path to the texture file.
		 */
		void SetSprite(const std::string &path);

		/**
		 * @brief Set the size of the billboard quad.
		 * @param size New size (width, height) in world units.
		 */
		void SetSize(const glm::vec2 &size);

		/**
		 * @brief Render the billboard quad facing the camera.
		 * @param shader Shader to use (should support textured quads).
		 * @param viewMatrix Camera view matrix.
		 * @param mode Current render mode (e.g., Default, Unlit).
		 */
		void Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode);

	private:
		/**
		 * @brief Initialize the static quad mesh if not already created.
		 */
		static void InitQuad();

		std::shared_ptr<Texture> m_SpriteTexture; ///< Sprite texture.
		glm::vec2 m_Size;						  ///< Billboard size (width, height).

		static std::unique_ptr<Mesh> s_QuadMesh; ///< Shared quad mesh for all billboards.
	};

} // namespace Engine
