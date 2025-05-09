/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:01:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:53:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/GPUResources/Framebuffer.h"
#include <glad/glad.h>
#include <iostream>

namespace Engine {

	Framebuffer::Framebuffer(unsigned int w, unsigned int h)
		: m_Width(w), m_Height(h) {
		glGenFramebuffers(1, &m_FBO);
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers(1, &m_FBO);
		for (auto tex : m_ColorAttachments)
			glDeleteTextures(1, &tex);
		if (m_RBO)
			glDeleteRenderbuffers(1, &m_RBO);
	}

	void Framebuffer::AddColorTexture(unsigned int internalFormat, unsigned int format, unsigned int type) {
		// Create and configure a color attachment texture
		unsigned int tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_ColorAttachments.push_back(tex);
	}

	void Framebuffer::AddDepthStencil() {
		// Create a depth-stencil renderbuffer attachment
		glGenRenderbuffers(1, &m_RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	}

	void Framebuffer::Build() {
		// Attach all color textures and optional depth-stencil to the FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
		}

		if (m_RBO) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
		}

		// Specify draw buffers for multiple render targets
		std::vector<unsigned int> drawBuffers;
		for (unsigned int i = 0; i < m_ColorAttachments.size(); ++i)
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void Framebuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int Framebuffer::GetColorAttachment(unsigned int idx) const {
		return m_ColorAttachments.at(idx);
	}

} // namespace Engine
