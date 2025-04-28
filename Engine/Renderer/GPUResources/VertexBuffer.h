/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexBuffer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:50:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	/**
	 * @brief Abstraction for an OpenGL Vertex Buffer Object (VBO).
	 *
	 * Manages GPU-side vertex buffer memory. Use Bind() before configuring vertex attributes.
	 * Automatically releases GPU resources on destruction.
	 */
	class VertexBuffer {
	public:
		/**
		 * @brief Creates a vertex buffer and uploads data to the GPU.
		 * @param vertices Pointer to vertex data (array of floats).
		 * @param size Size of the data in bytes.
		 */
		VertexBuffer(float *vertices, unsigned int size);

		/**
		 * @brief Destroys the vertex buffer and frees GPU resources.
		 */
		~VertexBuffer();

		/**
		 * @brief Binds this vertex buffer for use (GL_ARRAY_BUFFER).
		 */
		void Bind() const;

		/**
		 * @brief Unbinds any vertex buffer from GL_ARRAY_BUFFER.
		 */
		void Unbind() const;

	private:
		unsigned int m_RendererID; ///< OpenGL buffer object handle.
	};

} // namespace Engine
