/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:01:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:03:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PostProcessor.h
#pragma once
#include "Framebuffer.h"
#include <functional>
#include <memory>

namespace Engine {

	class Shader;

	class PostProcessor {
	public:
		PostProcessor(unsigned int width, unsigned int height);
		~PostProcessor();

		// 1. Génère le framebuffer HDR (MRT : [0]=scene, [1]=bright)
		// 2. Applique un blur gaussien sur bright pass
		// 3. Compose bloom + scene + tone map + gamma
		void Render(std::function<void()> sceneRender);

	private:
		unsigned int m_Width, m_Height;
		std::unique_ptr<Framebuffer> m_HDRFBO;
		std::unique_ptr<Framebuffer> m_PingPongFBO[2];

		std::unique_ptr<Shader> m_ScreenQuadShader;
		std::unique_ptr<Shader> m_BloomExtractShader;
		std::unique_ptr<Shader> m_GaussianBlurShader;
		std::unique_ptr<Shader> m_FinalShader;

		unsigned int m_QuadVAO = 0, m_QuadVBO = 0;
		bool m_First = true;

		void initRenderData();
		void renderQuad();
	};

} // namespace Engine
