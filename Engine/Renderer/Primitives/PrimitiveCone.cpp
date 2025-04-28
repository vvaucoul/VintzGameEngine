/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveCone.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:04 by vvaucoul          #+#    #+#             */
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

	std::unique_ptr<Mesh> Primitives::CreateCone(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		float halfHeight = height * 0.5f;

		// Apex vertex
		glm::vec3 apexPos(0.0f, halfHeight, 0.0f);
		glm::vec3 apexNormal(0.0f, 1.0f, 0.0f);	 // Normal points up, might need averaging later
		glm::vec3 apexTangent(1.0f, 0.0f, 0.0f); // Arbitrary
		glm::vec3 apexBitangent(0.0f, 0.0f, 1.0f);
		vertices.push_back({apexPos, apexNormal, {0.5f, 1.0f}, apexTangent, apexBitangent});
		unsigned int apexIndex = 0;

		// Base center vertex
		glm::vec3 baseCenterPos(0.0f, -halfHeight, 0.0f);
		glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
		glm::vec3 baseTangent(1.0f, 0.0f, 0.0f); // Arbitrary
		glm::vec3 baseBitangent(0.0f, 0.0f, 1.0f);
		vertices.push_back({baseCenterPos, baseNormal, {0.5f, 0.5f}, baseTangent, baseBitangent});
		unsigned int baseCenterIndex = 1;

		unsigned int baseVertexIndex = 2; // Starting index for ring vertices

		// Generate ring vertices (base ring, side ring)
		for (unsigned int i = 0; i <= sectorCount; ++i) {
			float angle = (float)i / sectorCount * 2.0f * M_PI;
			float x		= radius * cosf(angle);
			float z		= radius * sinf(angle);
			float u		= (float)i / sectorCount;

			// Base vertex
			glm::vec3 basePos(x, -halfHeight, z);
			glm::vec3 baseRingTangent	= glm::normalize(glm::vec3(-z, 0.0f, x)); // Tangent along circle
			glm::vec3 baseRingBitangent = glm::cross(baseNormal, baseRingTangent);
			glm::vec2 baseUV((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
			vertices.push_back({basePos, baseNormal, baseUV, baseRingTangent, baseRingBitangent}); // Base ring vertex (Index: baseVertexIndex + i*2 + 0)

			// Side vertex (calculate normal for the slanted surface)
			glm::vec3 sidePos		= basePos; // Side vertex shares position with base vertex
			glm::vec3 edge			= glm::normalize(apexPos - sidePos);
			glm::vec3 sideNormal	= glm::normalize(glm::cross(baseRingTangent, edge)); // Normal perpendicular to edge and tangent
			glm::vec3 sideTangent	= baseRingTangent;									 // Use the same tangent as the base ring
			glm::vec3 sideBitangent = glm::normalize(glm::cross(sideNormal, sideTangent));
			glm::vec2 sideUV(u, 0.0f);
			vertices.push_back({sidePos, sideNormal, sideUV, sideTangent, sideBitangent}); // Side ring vertex (Index: baseVertexIndex + i*2 + 1)
		}

		// Generate indices
		for (unsigned int i = 0; i < sectorCount; ++i) {
			unsigned int base_curr = baseVertexIndex + i * 2 + 0;
			unsigned int side_curr = baseVertexIndex + i * 2 + 1;

			// Use i+1 directly for next index, as we generated sectorCount+1 vertices
			unsigned int next_i	   = (i + 1);
			unsigned int base_next = baseVertexIndex + next_i * 2 + 0;
			unsigned int side_next = baseVertexIndex + next_i * 2 + 1;

			// Base triangle
			indices.push_back(baseCenterIndex);
			indices.push_back(base_next);
			indices.push_back(base_curr);

			// Side triangle
			indices.push_back(apexIndex);
			indices.push_back(side_next);
			indices.push_back(side_curr);
		}

		// Refine normals/tangents at apex? For cones, the normal/tangent is discontinuous at the apex.
		// Averaging might smooth it undesirably. Using the calculated side normals is common.
		// Could potentially duplicate the apex vertex for each side triangle with the corresponding side normal.

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
