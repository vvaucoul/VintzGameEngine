/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:50:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	class VertexBuffer;

	/**
	 * @brief Abstraction for an OpenGL Vertex Array Object (VAO).
	 *
	 * Manages the binding of vertex buffers and attribute layouts.
	 * Use Bind() before issuing draw calls. Automatically releases GPU resources on destruction.
	 */
	class VertexArray {
	public:
		/**
		 * @brief Creates a new vertex array object (VAO).
		 */
		VertexArray();

		/**
		 * @brief Destroys the VAO and frees GPU resources.
		 */
		~VertexArray();

		/**
		 * @brief Binds this VAO for use.
		 */
		void Bind() const;

		/**
		 * @brief Unbinds any VAO from the OpenGL context.
		 */
		void Unbind() const;

		/**
		 * @brief Attaches a vertex buffer to this VAO.
		 * @param vertexBuffer Reference to a VertexBuffer to bind.
		 *
		 * Note: Attribute layout must be set after binding the VAO and before drawing.
		 */
		void AddVertexBuffer(const VertexBuffer &vertexBuffer);

	private:
		unsigned int m_RendererID; ///< OpenGL VAO handle.
	};

} // namespace Engine
