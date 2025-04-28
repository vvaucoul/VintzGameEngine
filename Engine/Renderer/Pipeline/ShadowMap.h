/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShadowMap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:14:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 11:12:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ShadowMap.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

	/**
	 * @class ShadowMap
	 * @brief Encapsulates OpenGL resources and logic for shadow mapping with directional lights.
	 *
	 * ShadowMap manages a framebuffer object (FBO) and a depth texture used to render the scene
	 * from the light's perspective, producing a depth map for shadow calculations. It provides
	 * methods to bind the FBO for writing (rendering the shadow map), bind the depth texture for
	 * reading (sampling during scene rendering), and compute the light space transformation matrix.
	 *
	 * Typical usage:
	 *  - Call BindForWriting() before rendering the scene from the light's point of view.
	 *  - Use GetDepthMapTexture() to bind the shadow map as a sampler in your shaders.
	 *  - Use ComputeLightSpaceMatrix() to update the light's projection-view matrix.
	 */
	class ShadowMap {
	public:
		/**
		 * @brief Constructs a ShadowMap with the given resolution.
		 * @param width  Width of the shadow map texture (default: 2048).
		 * @param height Height of the shadow map texture (default: 2048).
		 */
		ShadowMap(unsigned int width = 2048, unsigned int height = 2048);

		/**
		 * @brief Destructor. Cleans up OpenGL resources.
		 */
		~ShadowMap();

		/**
		 * @brief Binds the framebuffer for writing depth from the light's perspective.
		 *
		 * Call this before rendering the scene to the shadow map.
		 */
		void BindForWriting();

		/**
		 * @brief Binds the depth texture for reading in shaders.
		 * @param textureUnit The OpenGL texture unit to bind to (e.g., GL_TEXTURE0).
		 *
		 * Call this before rendering the main scene to sample the shadow map.
		 */
		void BindForReading(GLenum textureUnit);

		/**
		 * @brief Computes the light space transformation matrix for a directional light.
		 * @param lightDir The direction vector of the light (world space, normalized).
		 *
		 * Updates the internal lightSpaceMatrix used for shadow mapping.
		 */
		void ComputeLightSpaceMatrix(const glm::vec3 &lightDir);

		/**
		 * @brief Returns the current light space transformation matrix.
		 * @return Reference to the glm::mat4 light space matrix.
		 */
		const glm::mat4 &GetLightSpaceMatrix() const { return lightSpaceMatrix; }

		/**
		 * @brief Returns the OpenGL texture ID of the depth map.
		 * @return GLuint texture ID.
		 */
		unsigned int GetDepthMapTexture() const { return depthMap; }

	private:
		unsigned int depthMapFBO;		  ///< OpenGL framebuffer object for shadow map rendering.
		unsigned int depthMap;			  ///< OpenGL texture ID for the depth map.
		const unsigned int SHADOW_WIDTH;  ///< Width of the shadow map texture.
		const unsigned int SHADOW_HEIGHT; ///< Height of the shadow map texture.
		glm::mat4 lightSpaceMatrix;		  ///< Light's projection-view matrix for shadow mapping.
	};

} // namespace Engine
