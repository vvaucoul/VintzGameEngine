/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveSphere.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:25:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <cmath> // For M_PI, cosf, sinf
#include <memory>
#include <vector>

// Define M_PI if not already defined (might be needed on some compilers/platforms)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine {

	std::unique_ptr<Mesh> Primitives::CreateSphere(unsigned int sectorCount, unsigned int stackCount) {
		std::vector<Vertex> verts;
		std::vector<unsigned int> idx;
		float radius = 0.5f;

		verts.reserve((stackCount + 1) * (sectorCount + 1));
		idx.reserve(stackCount * sectorCount * 6);

		for (unsigned int i = 0; i <= stackCount; ++i) {
			float stackAngle = M_PI / 2.0f - (float)i / stackCount * M_PI; // From +pi/2 to -pi/2
			float xy		 = radius * cosf(stackAngle);
			float y			 = radius * sinf(stackAngle);

			for (unsigned int j = 0; j <= sectorCount; ++j) {
				float sectorAngle = (float)j / sectorCount * 2.0f * M_PI; // From 0 to 2pi
				float x			  = xy * cosf(sectorAngle);
				float z			  = xy * sinf(sectorAngle);

				glm::vec3 pos(x, y, z);
				glm::vec3 normal = glm::normalize(pos);
				glm::vec2 uv((float)j / sectorCount, (float)i / stackCount);

				// Calculate tangent and bitangent
				// Tangent is derivative with respect to sector angle (phi)
				glm::vec3 tangent(-radius * sinf(stackAngle) * sinf(sectorAngle), 0, radius * sinf(stackAngle) * cosf(sectorAngle));
				// Bitangent is derivative with respect to stack angle (theta)
				glm::vec3 bitangent(radius * cosf(stackAngle) * cosf(sectorAngle), -radius * sinf(stackAngle), radius * cosf(stackAngle) * sinf(sectorAngle));

				// Handle poles where tangent might be zero or undefined
				if (i == 0 || i == stackCount) {
					tangent	  = glm::vec3(1.0f, 0.0f, 0.0f); // Assign arbitrary tangent at poles
					bitangent = glm::cross(normal, tangent);
				} else {
					tangent	  = glm::normalize(tangent);
					bitangent = glm::normalize(bitangent);
					// Optional: Re-orthogonalize using cross product
					// tangent = glm::normalize(glm::cross(bitangent, normal));
					// bitangent = glm::normalize(glm::cross(normal, tangent));
				}

				verts.push_back({pos, normal, uv, tangent, bitangent});
			}
		}

		// Indices remain the same logic
		for (unsigned int i = 0; i < stackCount; ++i) {
			unsigned int k1 = i * (sectorCount + 1);
			unsigned int k2 = k1 + sectorCount + 1;
			for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
				// 2 triangles per sector excluding first and last stacks
				if (i != 0) {
					idx.push_back(k1);
					idx.push_back(k2);
					idx.push_back(k1 + 1);
				}
				if (i != (stackCount - 1)) {
					idx.push_back(k1 + 1);
					idx.push_back(k2);
					idx.push_back(k2 + 1);
				}
			}
		}
		return std::make_unique<Mesh>(verts, idx);
	}

} // namespace Engine
