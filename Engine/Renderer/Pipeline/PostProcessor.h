/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:01:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:06:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Renderer/GPUResources/Framebuffer.h"
#include <functional>
#include <memory>

namespace Engine {

	class Shader;

	// Handles post-processing effects: HDR, bloom, tone mapping, gamma correction
	class PostProcessor {
	public:
		PostProcessor(unsigned int width, unsigned int height);
		~PostProcessor();

		// Runs the post-processing pipeline:
		// 1. Renders the scene to HDR framebuffer (with color + brightness targets)
		// 2. Applies Gaussian blur to the bright pass for bloom
		// 3. Composites scene + bloom, applies tone mapping and gamma correction
		void Render(std::function<void()> sceneRender);

	private:
		unsigned int m_Width, m_Height;

		// Framebuffers: HDR (scene + bright), ping-pong for blur passes
		std::unique_ptr<Framebuffer> m_HDRFBO;
		std::unique_ptr<Framebuffer> m_PingPongFBO[2];

		// Shaders for each post-process stage
		std::unique_ptr<Shader> m_ScreenQuadShader;
		std::unique_ptr<Shader> m_BloomExtractShader;
		std::unique_ptr<Shader> m_GaussianBlurShader;
		std::unique_ptr<Shader> m_FinalShader;

		// Fullscreen quad geometry
		unsigned int m_QuadVAO = 0, m_QuadVBO = 0;
		bool m_First = true;

		void initRenderData(); // Sets up quad VAO/VBO
		void renderQuad();	   // Draws fullscreen quad
	};

} // namespace Engine
