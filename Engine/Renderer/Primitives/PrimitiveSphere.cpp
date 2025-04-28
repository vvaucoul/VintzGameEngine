/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveSphere.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:11:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <cmath>
#include <memory>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine {

	std::unique_ptr<Mesh> Primitives::CreateSphere(unsigned int sectorCount, unsigned int stackCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		const float radius = 0.5f;

		vertices.reserve((stackCount + 1) * (sectorCount + 1));
		indices.reserve(stackCount * sectorCount * 6);

		// Generate vertices
		for (unsigned int stack = 0; stack <= stackCount; ++stack) {
			float stackTheta = M_PI_2 - (float(stack) / stackCount) * M_PI; // +pi/2 to -pi/2
			float xy		 = radius * cosf(stackTheta);
			float y			 = radius * sinf(stackTheta);

			for (unsigned int sector = 0; sector <= sectorCount; ++sector) {
				float sectorPhi = (float(sector) / sectorCount) * 2.0f * M_PI; // 0 to 2pi
				float x			= xy * cosf(sectorPhi);
				float z			= xy * sinf(sectorPhi);

				glm::vec3 pos(x, y, z);
				glm::vec3 normal = glm::normalize(pos);
				glm::vec2 uv(float(sector) / sectorCount, float(stack) / stackCount);

				// Tangent points along increasing phi (longitude)
				glm::vec3 tangent(-sinf(sectorPhi), 0.0f, cosf(sectorPhi));
				// Bitangent points along increasing theta (latitude)
				glm::vec3 bitangent = glm::cross(normal, tangent);

				// Handle poles: tangent/bitangent can be undefined
				if (stack == 0 || stack == stackCount) {
					tangent	  = glm::vec3(1.0f, 0.0f, 0.0f);
					bitangent = glm::cross(normal, tangent);
				} else {
					tangent	  = glm::normalize(tangent);
					bitangent = glm::normalize(bitangent);
				}

				vertices.push_back({pos, normal, uv, tangent, bitangent});
			}
		}

		// Generate indices
		for (unsigned int stack = 0; stack < stackCount; ++stack) {
			unsigned int k1 = stack * (sectorCount + 1);
			unsigned int k2 = k1 + sectorCount + 1;
			for (unsigned int sector = 0; sector < sectorCount; ++sector, ++k1, ++k2) {
				if (stack != 0) {
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}
				if (stack != stackCount - 1) {
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
