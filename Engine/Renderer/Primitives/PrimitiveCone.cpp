/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveCone.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:57:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Geometry/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <cmath>
#include <memory>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine {

	std::unique_ptr<Mesh> Primitives::CreateCone(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		const float halfHeight = height * 0.5f;

		// Apex vertex (top point)
		const glm::vec3 apexPos(0.0f, halfHeight, 0.0f);
		vertices.push_back({apexPos,
							glm::vec3(0.0f, 1.0f, 0.0f), // Upward normal (not used for shading, just placeholder)
							{0.5f, 1.0f},
							glm::vec3(1.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f)});
		const unsigned int apexIndex = 0;

		// Base center vertex
		const glm::vec3 baseCenterPos(0.0f, -halfHeight, 0.0f);
		vertices.push_back({baseCenterPos,
							glm::vec3(0.0f, -1.0f, 0.0f), // Downward normal
							{0.5f, 0.5f},
							glm::vec3(1.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f)});
		const unsigned int baseCenterIndex = 1;

		const unsigned int baseVertexStart = 2;

		// Generate base ring and side ring vertices
		for (unsigned int i = 0; i <= sectorCount; ++i) {
			const float theta = (float)i / sectorCount * 2.0f * M_PI;
			const float x	  = radius * std::cos(theta);
			const float z	  = radius * std::sin(theta);
			const float u	  = (float)i / sectorCount;

			// Base ring vertex
			glm::vec3 basePos(x, -halfHeight, z);
			glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
			glm::vec3 baseTangent	= glm::normalize(glm::vec3(-z, 0.0f, x));
			glm::vec3 baseBitangent = glm::cross(baseNormal, baseTangent);
			glm::vec2 baseUV((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
			vertices.push_back({basePos, baseNormal, baseUV, baseTangent, baseBitangent});

			// Side ring vertex (shares position with base, but normal is slanted)
			glm::vec3 sideNormal	= glm::normalize(glm::vec3(x, radius / height, z));
			glm::vec3 sideTangent	= baseTangent;
			glm::vec3 sideBitangent = glm::cross(sideNormal, sideTangent);
			glm::vec2 sideUV(u, 0.0f);
			vertices.push_back({basePos, sideNormal, sideUV, sideTangent, sideBitangent});
		}

		// Indices for base (fan)
		for (unsigned int i = 0; i < sectorCount; ++i) {
			unsigned int curr = baseVertexStart + i * 2;
			unsigned int next = baseVertexStart + ((i + 1) % (sectorCount + 1)) * 2;
			indices.push_back(baseCenterIndex);
			indices.push_back(next);
			indices.push_back(curr);
		}

		// Indices for sides (apex to side ring)
		for (unsigned int i = 0; i < sectorCount; ++i) {
			unsigned int curr = baseVertexStart + i * 2 + 1;
			unsigned int next = baseVertexStart + ((i + 1) % (sectorCount + 1)) * 2 + 1;
			indices.push_back(apexIndex);
			indices.push_back(curr);
			indices.push_back(next);
		}

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
