/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:00:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:00:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Framebuffer.h
#pragma once
#include <vector>

namespace Engine {

	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height);
		~Framebuffer();

		// Attache un color‐attachment (texture)
		void AddColorTexture(unsigned int internalFormat, unsigned int format, unsigned int type);
		// Attache un depth‐stencil buffer
		void AddDepthStencil();

		// Finalise et vérifie l’état
		void Build();

		void Bind();
		void Unbind();

		unsigned int GetColorAttachment(unsigned int idx = 0) const;

	private:
		unsigned int m_FBO = 0;
		unsigned int m_Width, m_Height;
		std::vector<unsigned int> m_ColorAttachments;
		unsigned int m_RBO = 0; // pour depth/stencil
	};

} // namespace Engine
