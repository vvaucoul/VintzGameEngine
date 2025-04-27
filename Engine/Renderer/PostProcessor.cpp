/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcessor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:01:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 22:47:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/PostProcessor.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Shader.h"
#include <functional>
#include <glad/glad.h>
#include <iostream>
#include <memory>

namespace Engine {

	PostProcessor::PostProcessor(unsigned w, unsigned h)
		: m_Width(w), m_Height(h) {
		// 1) HDR FBO : deux color attachments (scene + bright)
		m_HDRFBO = std::make_unique<Framebuffer>(w, h);
		m_HDRFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_HDRFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
		m_HDRFBO->AddDepthStencil();
		m_HDRFBO->Build();

		// 2) Ping-Pong FBO pour blur
		for (int i = 0; i < 2; ++i) {
			m_PingPongFBO[i] = std::make_unique<Framebuffer>(w, h);
			m_PingPongFBO[i]->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
			m_PingPongFBO[i]->Build();
		}

		// 3) Charger les shaders
		// Update paths to PostProcess directory
		m_BloomExtractShader = std::make_unique<Shader>("Shaders/PostProcess/bloom_extract.vert",
														"Shaders/PostProcess/bloom_extract.frag");
		m_GaussianBlurShader = std::make_unique<Shader>("Shaders/PostProcess/blur.vert",
														"Shaders/PostProcess/blur.frag");
		m_FinalShader		 = std::make_unique<Shader>("Shaders/PostProcess/final.vert",
													"Shaders/PostProcess/final.frag");

		initRenderData();
	}

	PostProcessor::~PostProcessor() {
		glDeleteVertexArrays(1, &m_QuadVAO);
		glDeleteBuffers(1, &m_QuadVBO);
	}

	void PostProcessor::Render(std::function<void()> sceneRender) {
		// Ensure depth test is enabled for scene rendering if needed
		glEnable(GL_DEPTH_TEST);

		// 1. Render scene into HDR framebuffer (with MRT)
		m_HDRFBO->Bind();
		// Specify draw buffers for MRT
		unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, attachments);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sceneRender(); // Call the provided function to render the scene
		m_HDRFBO->Unbind();

		// Disable depth test for subsequent 2D quad rendering
		glDisable(GL_DEPTH_TEST);

		// --- Bloom Pipeline ---
		bool horizontal		 = true;
		bool first_iteration = true;
		int amount			 = 10; // Number of blur passes (5 horizontal, 5 vertical)

		m_GaussianBlurShader->Bind();
		for (unsigned int i = 0; i < amount; i++) {
			m_PingPongFBO[horizontal]->Bind(); // Bind target ping-pong FBO
			m_GaussianBlurShader->SetUniformInt("horizontal", horizontal);

			glActiveTexture(GL_TEXTURE0);
			// Bind texture: In the first iteration, bind the HDR bright pass, otherwise bind the previous ping-pong result
			glBindTexture(GL_TEXTURE_2D, first_iteration ? m_HDRFBO->GetColorAttachment(1) : m_PingPongFBO[!horizontal]->GetColorAttachment(0));
			m_GaussianBlurShader->SetUniformInt("image", 0); // Assuming blur shader uses uniform "image"

			renderQuad(); // Render the quad to apply the blur

			m_PingPongFBO[horizontal]->Unbind(); // Unbind the target FBO

			horizontal = !horizontal; // Toggle direction
			if (first_iteration)
				first_iteration = false;
		}
		// At this point, m_PingPongFBO[!horizontal] contains the final blurred texture

		// 4) Final composite + tone mapping + gamma to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind default framebuffer
		glClear(GL_COLOR_BUFFER_BIT);		  // Only need to clear color
		m_FinalShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HDRFBO->GetColorAttachment(0)); // scene color
		m_FinalShader->SetUniformInt("scene", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_PingPongFBO[!horizontal]->GetColorAttachment(0)); // blurred bloom texture
		m_FinalShader->SetUniformInt("bloomBlur", 1);
		m_FinalShader->SetUniformFloat("exposure", 1.0f); // Keep exposure at 1.0 for now
		renderQuad();

		// Reset texture bindings (optional but good practice)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void PostProcessor::initRenderData() {
		float quadVertices[] = {
			// positions   // texCoords
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
		// Ensure VAO is unbound after setup
		glBindVertexArray(0);
	}

	void PostProcessor::renderQuad() {
		glBindVertexArray(m_QuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

} // namespace Engine
