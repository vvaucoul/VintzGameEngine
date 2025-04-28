/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveCylinder.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:25:08 by vvaucoul         ###   ########.fr       */
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

	std::unique_ptr<Mesh> Primitives::CreateCylinder(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		float halfHeight = height * 0.5f;

		// Top cap center
		glm::vec3 topCenterPos(0.0f, halfHeight, 0.0f);
		glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
		glm::vec3 topTangent(1.0f, 0.0f, 0.0f); // Arbitrary for center
		glm::vec3 topBitangent(0.0f, 0.0f, 1.0f);
		vertices.push_back({topCenterPos, topNormal, {0.5f, 0.5f}, topTangent, topBitangent});
		unsigned int topCenterIndex = 0;

		// Bottom cap center
		glm::vec3 bottomCenterPos(0.0f, -halfHeight, 0.0f);
		glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
		glm::vec3 bottomTangent(1.0f, 0.0f, 0.0f); // Arbitrary for center
		glm::vec3 bottomBitangent(0.0f, 0.0f, 1.0f);
		vertices.push_back({bottomCenterPos, bottomNormal, {0.5f, 0.5f}, bottomTangent, bottomBitangent});
		unsigned int bottomCenterIndex = 1;

		unsigned int baseVertexIndex = 2; // Starting index for ring vertices

		// Generate ring vertices (top cap, bottom cap, top side, bottom side)
		for (unsigned int i = 0; i <= sectorCount; ++i) {
			float angle = (float)i / sectorCount * 2.0f * M_PI;
			float x		= radius * cosf(angle);
			float z		= radius * sinf(angle);
			float u		= (float)i / sectorCount;

			// Cap vertices
			glm::vec3 capTangent		 = glm::normalize(glm::vec3(-z, 0.0f, x)); // Tangent along circle
			glm::vec3 capBitangentTop	 = glm::cross(topNormal, capTangent);
			glm::vec3 capBitangentBottom = glm::cross(bottomNormal, capTangent);
			glm::vec2 capUV((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
			vertices.push_back({{x, halfHeight, z}, topNormal, capUV, capTangent, capBitangentTop});		// Top cap ring
			vertices.push_back({{x, -halfHeight, z}, bottomNormal, capUV, capTangent, capBitangentBottom}); // Bottom cap ring

			// Side vertices
			glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));
			glm::vec3 sideTangent(0.0f, 1.0f, 0.0f); // Along the height
			glm::vec3 sideBitangent = glm::normalize(glm::cross(sideNormal, sideTangent));
			vertices.push_back({{x, halfHeight, z}, sideNormal, {u, 1.0f}, sideTangent, sideBitangent});  // Top side ring
			vertices.push_back({{x, -halfHeight, z}, sideNormal, {u, 0.0f}, sideTangent, sideBitangent}); // Bottom side ring
		}

		// Generate indices
		for (unsigned int i = 0; i < sectorCount; ++i) {
			// Indices for the 4 vertices generated in the loop for sector 'i'
			unsigned int tc_curr = baseVertexIndex + i * 4 + 0; // Top Cap current
			unsigned int bc_curr = baseVertexIndex + i * 4 + 1; // Bottom Cap current
			unsigned int ts_curr = baseVertexIndex + i * 4 + 2; // Top Side current
			unsigned int bs_curr = baseVertexIndex + i * 4 + 3; // Bottom Side current

			// Indices for the 4 vertices generated in the loop for sector 'i+1'
			// Need to handle wrap-around for the last sector connecting to the first
			unsigned int next_i	 = (i + 1); // Use i+1 directly because we generated sectorCount+1 vertices
			unsigned int tc_next = baseVertexIndex + next_i * 4 + 0;
			unsigned int bc_next = baseVertexIndex + next_i * 4 + 1;
			unsigned int ts_next = baseVertexIndex + next_i * 4 + 2;
			unsigned int bs_next = baseVertexIndex + next_i * 4 + 3;

			// Top cap triangle
			indices.push_back(topCenterIndex);
			indices.push_back(tc_next);
			indices.push_back(tc_curr);

			// Bottom cap triangle
			indices.push_back(bottomCenterIndex);
			indices.push_back(bc_curr);
			indices.push_back(bc_next);

			// Side quad (two triangles)
			indices.push_back(ts_curr);
			indices.push_back(ts_next);
			indices.push_back(bs_curr);

			indices.push_back(ts_next);
			indices.push_back(bs_next);
			indices.push_back(bs_curr);
		}

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
