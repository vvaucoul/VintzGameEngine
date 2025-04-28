/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:01:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:05:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Pipeline/PostProcessor.h"
#include "Renderer/GPUResources/Framebuffer.h"
#include "Renderer/Shaders/Shader.h"
#include <functional>
#include <glad/glad.h>
#include <iostream>
#include <memory>

namespace Engine {

	PostProcessor::PostProcessor(unsigned w, unsigned h)
		: m_Width(w), m_Height(h) {
		// Create HDR framebuffer with two color attachments: scene color and brightness (for bloom)
		m_HDRFBO = std::make_unique<Framebuffer>(w, h);
		m_HDRFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT); // scene color
		m_HDRFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT); // brightness
		m_HDRFBO->AddDepthStencil();
		m_HDRFBO->Build();

		// Create two ping-pong framebuffers for Gaussian blur passes
		for (int i = 0; i < 2; ++i) {
			m_PingPongFBO[i] = std::make_unique<Framebuffer>(w, h);
			m_PingPongFBO[i]->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
			m_PingPongFBO[i]->Build();
		}

		// Load post-process shaders
		m_BloomExtractShader = std::make_unique<Shader>(
			"Shaders/PostProcess/bloom_extract.vert",
			"Shaders/PostProcess/bloom_extract.frag");
		m_GaussianBlurShader = std::make_unique<Shader>(
			"Shaders/PostProcess/blur.vert",
			"Shaders/PostProcess/blur.frag");
		m_FinalShader = std::make_unique<Shader>(
			"Shaders/PostProcess/final.vert",
			"Shaders/PostProcess/final.frag");

		initRenderData();
	}

	PostProcessor::~PostProcessor() {
		glDeleteVertexArrays(1, &m_QuadVAO);
		glDeleteBuffers(1, &m_QuadVBO);
	}

	void PostProcessor::Render(std::function<void()> sceneRender) {
		glEnable(GL_DEPTH_TEST);

		// Render the scene to HDR framebuffer (multiple render targets)
		m_HDRFBO->Bind();
		unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, attachments);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sceneRender();
		m_HDRFBO->Unbind();

		glDisable(GL_DEPTH_TEST);

		// Gaussian blur: alternate horizontal/vertical passes using ping-pong FBOs
		bool horizontal		 = true;
		bool first_iteration = true;
		const int blurPasses = 10;

		m_GaussianBlurShader->Bind();
		for (int i = 0; i < blurPasses; ++i) {
			m_PingPongFBO[horizontal]->Bind();
			m_GaussianBlurShader->SetUniformInt("horizontal", horizontal);

			glActiveTexture(GL_TEXTURE0);
			// First pass uses brightness texture, subsequent passes use previous blur result
			glBindTexture(GL_TEXTURE_2D, first_iteration ? m_HDRFBO->GetColorAttachment(1) : m_PingPongFBO[!horizontal]->GetColorAttachment(0));
			m_GaussianBlurShader->SetUniformInt("image", 0);

			renderQuad();

			m_PingPongFBO[horizontal]->Unbind();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		// Final blurred result is in m_PingPongFBO[!horizontal]

		// Composite: combine scene color and blurred bloom, apply tone mapping/gamma
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		m_FinalShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HDRFBO->GetColorAttachment(0)); // scene color
		m_FinalShader->SetUniformInt("scene", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_PingPongFBO[!horizontal]->GetColorAttachment(0)); // bloom
		m_FinalShader->SetUniformInt("bloomBlur", 1);
		m_FinalShader->SetUniformFloat("exposure", 1.0f);

		renderQuad();

		// Unbind textures for cleanliness
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void PostProcessor::initRenderData() {
		// Fullscreen quad: 2D positions and UVs
		float quadVertices[] = {
			// pos    // uv
			-1,
			-1,
			0,
			0,
			1,
			-1,
			1,
			0,
			-1,
			1,
			0,
			1,
			-1,
			1,
			0,
			1,
			1,
			-1,
			1,
			0,
			1,
			1,
			1,
			1,
		};
		glGenVertexArrays(1, &m_QuadVAO);
		glGenBuffers(1, &m_QuadVBO);
		glBindVertexArray(m_QuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
		glBindVertexArray(0);
	}

	void PostProcessor::renderQuad() {
		glBindVertexArray(m_QuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

} // namespace Engine
