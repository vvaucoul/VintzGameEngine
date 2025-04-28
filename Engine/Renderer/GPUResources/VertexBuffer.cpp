/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexBuffer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:49:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VertexBuffer.h"
#include <glad/glad.h>

namespace Engine {

	// Create a vertex buffer and upload data to GPU
	VertexBuffer::VertexBuffer(float *vertices, unsigned int size) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	// Release GPU buffer
	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	// Bind this buffer for use
	void VertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	// Unbind any vertex buffer
	void VertexBuffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // namespace Engine
