/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShadowMap.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:14:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:06:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Pipeline/ShadowMap.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	ShadowMap::ShadowMap(unsigned int w, unsigned int h)
		: SHADOW_WIDTH(w), SHADOW_HEIGHT(h) {
		// Create framebuffer for shadow mapping
		glGenFramebuffers(1, &depthMapFBO);

		// Create depth texture for storing shadow map
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// Attach depth texture to framebuffer (no color buffer)
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	ShadowMap::~ShadowMap() {
		glDeleteFramebuffers(1, &depthMapFBO);
		glDeleteTextures(1, &depthMap);
	}

	void ShadowMap::BindForWriting() {
		// Set viewport and bind framebuffer for shadow pass
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void ShadowMap::BindForReading(GLenum textureUnit) {
		// Bind shadow map texture for sampling in shaders
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	void ShadowMap::ComputeLightSpaceMatrix(const glm::vec3 &lightDir) {
		// Build orthographic projection and view matrix for directional light
		const float near_plane = 1.0f, far_plane = 25.0f;
		const float orthoSize = 10.0f;
		glm::mat4 lightProj	  = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
		glm::mat4 lightView	  = glm::lookAt(-lightDir * 10.0f, glm::vec3(0.0f), glm::vec3(0, 1, 0));
		lightSpaceMatrix	  = lightProj * lightView;
	}

} // namespace Engine
