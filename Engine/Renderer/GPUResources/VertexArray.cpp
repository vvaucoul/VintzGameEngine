/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:50:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VertexArray.h"
#include "VertexBuffer.h"
#include <glad/glad.h>

namespace Engine {

	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const {
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const VertexBuffer &vertexBuffer) {
		// Bind VAO and VBO before setting up attribute pointers
		Bind();
		vertexBuffer.Bind();

		// For now: assume a simple layout (position only, 3 floats per vertex)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,				   // Attribute index
			3,				   // Number of components (x, y, z)
			GL_FLOAT,		   // Type
			GL_FALSE,		   // Normalized?
			3 * sizeof(float), // Stride
			(void *)0		   // Offset
		);
	}

} // namespace Engine
