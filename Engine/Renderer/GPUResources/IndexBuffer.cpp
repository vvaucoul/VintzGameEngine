/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IndexBuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:54:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:52:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IndexBuffer.h"
#include <glad/glad.h>

namespace Engine {

	// Construct and upload index data to GPU
	IndexBuffer::IndexBuffer(unsigned int *indices, unsigned int count)
		: m_Count(count) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	// Free GPU buffer
	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	// Bind for indexed draw calls
	void IndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	// Unbind any index buffer
	void IndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

} // namespace Engine
