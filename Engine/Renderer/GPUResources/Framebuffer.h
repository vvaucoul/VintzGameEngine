/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:51:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <stdexcept>
#include <vector>

namespace Engine {

	/**
	 * @brief Abstraction for an OpenGL Framebuffer Object (FBO).
	 *
	 * Manages off-screen rendering targets, supporting multiple color attachments
	 * (textures) and optional depth/stencil renderbuffer. Use for post-processing,
	 * shadow mapping, or any render-to-texture workflow.
	 *
	 * Usage:
	 *   1. Create Framebuffer with width/height.
	 *   2. AddColorTexture() for each color attachment needed.
	 *   3. Optionally AddDepthStencil().
	 *   4. Call Build() to allocate and attach resources.
	 *   5. Bind()/Unbind() for rendering.
	 */
	class Framebuffer {
	public:
		/**
		 * @brief Construct a framebuffer with given dimensions.
		 * @param width  Width in pixels for all attachments.
		 * @param height Height in pixels for all attachments.
		 */
		Framebuffer(unsigned int width, unsigned int height);

		/**
		 * @brief Destructor. Releases all OpenGL resources.
		 */
		~Framebuffer();

		// Non-copyable, non-movable (RAII)
		Framebuffer(const Framebuffer &)			= delete;
		Framebuffer &operator=(const Framebuffer &) = delete;
		Framebuffer(Framebuffer &&)					= delete;
		Framebuffer &operator=(Framebuffer &&)		= delete;

		/**
		 * @brief Add a color texture attachment.
		 * Call before Build(). Each call adds a new color attachment.
		 * @param internalFormat OpenGL internal format (e.g. GL_RGBA8, GL_RGBA16F).
		 * @param format         Pixel format (e.g. GL_RGBA).
		 * @param type           Data type (e.g. GL_UNSIGNED_BYTE, GL_FLOAT).
		 */
		void AddColorTexture(GLenum internalFormat, GLenum format, GLenum type);

		/**
		 * @brief Add a depth+stencil renderbuffer attachment (GL_DEPTH24_STENCIL8).
		 * Call before Build(). Only one depth/stencil attachment is supported.
		 */
		void AddDepthStencil();

		/**
		 * @brief Finalize and allocate all attachments. Must be called after adding attachments.
		 * @throws std::runtime_error if framebuffer is incomplete.
		 */
		void Build();

		/**
		 * @brief Bind this framebuffer for rendering.
		 */
		void Bind();

		/**
		 * @brief Unbind (bind default framebuffer, id 0).
		 */
		void Unbind();

		/**
		 * @brief Get OpenGL texture ID of a color attachment.
		 * @param idx Index of the color attachment (default: 0).
		 * @return Texture ID.
		 * @throws std::out_of_range if idx is invalid.
		 */
		GLuint GetColorAttachment(unsigned int idx = 0) const;

		/**
		 * @brief Get OpenGL FBO ID.
		 * @return FBO handle, or 0 if not built.
		 */
		GLuint GetID() const;

		/**
		 * @brief Get framebuffer width in pixels.
		 */
		unsigned int GetWidth() const;

		/**
		 * @brief Get framebuffer height in pixels.
		 */
		unsigned int GetHeight() const;

	private:
		GLuint m_FBO = 0;						///< OpenGL Framebuffer Object handle
		unsigned int m_Width, m_Height;			///< Dimensions of attachments
		std::vector<GLuint> m_ColorAttachments; ///< Texture IDs for color attachments
		GLuint m_RBO   = 0;						///< Renderbuffer for depth/stencil
		bool m_IsBuilt = false;					///< True if Build() succeeded
	};

	// --- Inline implementations ---

	inline GLuint Framebuffer::GetID() const {
		return m_FBO;
	}

	inline unsigned int Framebuffer::GetWidth() const {
		return m_Width;
	}

	inline unsigned int Framebuffer::GetHeight() const {
		return m_Height;
	}

} // namespace Engine
