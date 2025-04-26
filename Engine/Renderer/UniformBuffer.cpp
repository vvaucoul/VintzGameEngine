/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UniformBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:31:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 12:31:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UniformBuffer.h"
#include <glad/glad.h>

namespace Engine {

	UniformBuffer::UniformBuffer(unsigned int size, unsigned int binding) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		// Allocation sans données, usage dynamique
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		// Lie le buffer au point d'attache 'binding'
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBuffer::~UniformBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::Bind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	void UniformBuffer::Unbind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::SetData(unsigned int offset, unsigned int size, const void *data) const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

} // namespace Engine