/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UniformBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:31:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:01:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UniformBuffer.h"
#include <glad/glad.h>

namespace Engine {

	// Creates a UBO of given size and binds it to a binding point
	UniformBuffer::UniformBuffer(unsigned int size, unsigned int binding) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // Allocate buffer, no initial data
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);		 // Bind to binding point
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBuffer::~UniformBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	// Bind this UBO for updates or use
	void UniformBuffer::Bind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	// Unbind any UBO from the target
	void UniformBuffer::Unbind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Update a region of the buffer with new data
	void UniformBuffer::SetData(unsigned int offset, unsigned int size, const void *data) const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

} // namespace Engine