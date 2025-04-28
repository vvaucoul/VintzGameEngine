/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UniformBuffer.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:31:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:00:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/**
 * @file UniformBuffer.h
 * @brief OpenGL Uniform Buffer Object (UBO) abstraction for fast uniform updates.
 *
 * Encapsulates creation, binding, and data upload for OpenGL UBOs.
 * Use to share uniform data (matrices, lighting, etc.) efficiently across shaders.
 */

namespace Engine {

	/**
	 * @class UniformBuffer
	 * @brief RAII wrapper for OpenGL Uniform Buffer Objects (UBOs).
	 *
	 * Usage:
	 *   - Construct with buffer size (bytes) and binding point.
	 *   - Use SetData() to update buffer contents.
	 *   - Bind/Unbind as needed for manual updates.
	 */
	class UniformBuffer {
	public:
		/**
		 * @brief Create a Uniform Buffer Object.
		 * @param size    Size of the buffer in bytes.
		 * @param binding Binding point index (matches shader layout(binding = N)).
		 */
		UniformBuffer(unsigned int size, unsigned int binding);

		/**
		 * @brief Destroy the Uniform Buffer and free GPU resources.
		 */
		~UniformBuffer();

		/**
		 * @brief Bind this UBO to GL_UNIFORM_BUFFER for updates.
		 */
		void Bind() const;

		/**
		 * @brief Unbind any UBO from GL_UNIFORM_BUFFER.
		 */
		void Unbind() const;

		/**
		 * @brief Update a region of the buffer with new data.
		 * @param offset Byte offset into the buffer.
		 * @param size   Number of bytes to update.
		 * @param data   Pointer to source data.
		 */
		void SetData(unsigned int offset, unsigned int size, const void *data) const;

	private:
		unsigned int m_RendererID = 0; ///< OpenGL buffer object handle.
	};

} // namespace Engine