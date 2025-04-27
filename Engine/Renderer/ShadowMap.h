/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShadowMap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:14:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:41:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ShadowMap.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {
	class ShadowMap {
	public:
		ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
		~ShadowMap();

		// Prépare le FBO pour écrire la profondeur
		void BindForWriting();
		// Lie la depth texture sur l’unité donnée
		void BindForReading(GLenum textureUnit);

		// Calcule lightSpaceMatrix d’une directional light
		void ComputeLightSpaceMatrix(const glm::vec3 &lightDir);

		const glm::mat4 &GetLightSpaceMatrix() const { return lightSpaceMatrix; }
		unsigned int GetDepthMapTexture() const { return depthMap; } // Ajout de ce getter

	private:
		unsigned int depthMapFBO;
		unsigned int depthMap; // ID de la texture
		const unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
		glm::mat4 lightSpaceMatrix;
	};
}
