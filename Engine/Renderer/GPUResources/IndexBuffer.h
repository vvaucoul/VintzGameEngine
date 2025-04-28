/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IndexBuffer.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:51:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	/**
	 * @brief Abstraction for an OpenGL Index Buffer Object (IBO/EBO).
	 *
	 * Manages GPU-side index buffer memory for indexed drawing.
	 * Use Bind() before issuing draw calls that use element indices.
	 * Automatically releases GPU resources on destruction.
	 */
	class IndexBuffer {
	public:
		/**
		 * @brief Creates an index buffer and uploads index data to the GPU.
		 * @param indices Pointer to index data (array of unsigned ints).
		 * @param count Number of indices in the array.
		 */
		IndexBuffer(unsigned int *indices, unsigned int count);

		/**
		 * @brief Destroys the index buffer and frees GPU resources.
		 */
		~IndexBuffer();

		/**
		 * @brief Binds this index buffer for use with GL_ELEMENT_ARRAY_BUFFER.
		 */
		void Bind() const;

		/**
		 * @brief Unbinds any index buffer from GL_ELEMENT_ARRAY_BUFFER.
		 */
		void Unbind() const;

		/**
		 * @brief Returns the number of indices stored in this buffer.
		 * @return Number of indices.
		 */
		inline unsigned int GetCount() const { return m_Count; }

	private:
		unsigned int m_RendererID; ///< OpenGL buffer object handle.
		unsigned int m_Count;	   ///< Number of indices in the buffer.
	};

} // namespace Engine
