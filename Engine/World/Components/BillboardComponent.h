/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BillboardComponent.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:38:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:40:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Core/Application.h" // Include Application for RenderMode enum
#include "Renderer/Mesh.h"	  // Include Mesh for the quad
#include "Renderer/Textures/Texture.h"
#include "World/Components/SceneComponent.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Engine {

	class Shader;
	class Actor;

	/**
	 * @brief A component that renders a 2D texture always facing the camera.
	 * Inherits from SceneComponent to get a world transform.
	 */
	class BillboardComponent : public SceneComponent {
	public:
		/**
		 * @brief Constructor.
		 * @param owner The owning Actor.
		 * @param spritePath Optional path to the sprite texture file.
		 * @param size Initial size of the billboard (width, height).
		 */
		BillboardComponent(Actor *owner, const std::string &spritePath = "", const glm::vec2 &size = {1.0f, 1.0f});

		/// Destructor.
		~BillboardComponent() override = default;

		/**
		 * @brief Sets the texture used for the billboard sprite.
		 * @param texture A shared pointer to the Texture object.
		 */
		void SetSprite(std::shared_ptr<Texture> texture);

		/**
		 * @brief Loads and sets the texture used for the billboard sprite from a file path.
		 * @param path Path to the texture file.
		 */
		void SetSprite(const std::string &path);

		/**
		 * @brief Sets the size (width, height) of the billboard in world units.
		 * @param size The new size.
		 */
		void SetSize(const glm::vec2 &size);

		/**
		 * @brief Renders the billboard quad facing the camera.
		 * @param shader Shader to use (should handle textures, e.g., Unlit or PBR).
		 * @param viewMatrix Current camera view matrix.
		 * @param mode The current rendering mode (ignored by billboard for now, assumes Unlit/PBR).
		 */
		void Render(Shader &shader, const glm::mat4 &viewMatrix, RenderMode mode);

	private:
		std::shared_ptr<Texture> m_SpriteTexture; ///< The texture to display.
		glm::vec2 m_Size;						  ///< Size of the billboard quad.

		/**
		 * @brief Initializes the static quad mesh if not already done.
		 */
		static void InitQuad();

		static std::unique_ptr<Mesh> s_QuadMesh; ///< Shared quad mesh for all billboards.
	};

} // namespace Engine
