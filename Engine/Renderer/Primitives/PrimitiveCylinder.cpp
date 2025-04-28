/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveCylinder.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:09:38 by vvaucoul         ###   ########.fr       */
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

	// Generates a cylinder mesh centered at the origin, aligned along Y axis.
	// - height: total height of the cylinder
	// - radius: radius of the cylinder
	// - sectorCount: number of segments around the circumference (minimum 3)
	std::unique_ptr<Mesh> Primitives::CreateCylinder(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		const float halfHeight = height * 0.5f;

		// Add center vertices for top and bottom caps
		vertices.push_back({
			{0.0f, halfHeight, 0.0f}, // Position (top center)
			{0.0f, 1.0f, 0.0f},		  // Normal (up)
			{0.5f, 0.5f},			  // UV (center)
			{1.0f, 0.0f, 0.0f},		  // Tangent
			{0.0f, 0.0f, 1.0f}		  // Bitangent
		});
		vertices.push_back({
			{0.0f, -halfHeight, 0.0f}, // Position (bottom center)
			{0.0f, -1.0f, 0.0f},	   // Normal (down)
			{0.5f, 0.5f},			   // UV (center)
			{1.0f, 0.0f, 0.0f},		   // Tangent
			{0.0f, 0.0f, 1.0f}		   // Bitangent
		});
		const unsigned int topCenterIdx	   = 0;
		const unsigned int bottomCenterIdx = 1;
		const unsigned int ringStartIdx	   = 2;

		// Generate ring vertices for caps and sides
		for (unsigned int i = 0; i <= sectorCount; ++i) {
			const float theta		= (float)i / sectorCount * 2.0f * M_PI;
			const float x			= radius * std::cos(theta);
			const float z			= radius * std::sin(theta);
			const float u			= (float)i / sectorCount;
			const glm::vec3 tangent = glm::normalize(glm::vec3(-z, 0.0f, x));

			// Top cap ring vertex
			vertices.push_back({{x, halfHeight, z},
								{0.0f, 1.0f, 0.0f},
								{(x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f},
								tangent,
								glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent)});
			// Bottom cap ring vertex
			vertices.push_back({{x, -halfHeight, z},
								{0.0f, -1.0f, 0.0f},
								{(x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f},
								tangent,
								glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), tangent)});
			// Top side ring vertex
			vertices.push_back({{x, halfHeight, z},
								glm::normalize(glm::vec3(x, 0.0f, z)),
								{u, 1.0f},
								{0.0f, 1.0f, 0.0f},
								glm::normalize(glm::cross(glm::normalize(glm::vec3(x, 0.0f, z)), {0.0f, 1.0f, 0.0f}))});
			// Bottom side ring vertex
			vertices.push_back({{x, -halfHeight, z},
								glm::normalize(glm::vec3(x, 0.0f, z)),
								{u, 0.0f},
								{0.0f, 1.0f, 0.0f},
								glm::normalize(glm::cross(glm::normalize(glm::vec3(x, 0.0f, z)), {0.0f, 1.0f, 0.0f}))});
		}

		// Build indices for caps and sides
		for (unsigned int i = 0; i < sectorCount; ++i) {
			const unsigned int curr = ringStartIdx + i * 4;
			const unsigned int next = ringStartIdx + (i + 1) * 4;

			// Top cap triangle (fan)
			indices.push_back(topCenterIdx);
			indices.push_back(next + 0);
			indices.push_back(curr + 0);

			// Bottom cap triangle (fan)
			indices.push_back(bottomCenterIdx);
			indices.push_back(curr + 1);
			indices.push_back(next + 1);

			// Side quad (two triangles)
			indices.push_back(curr + 2);
			indices.push_back(next + 2);
			indices.push_back(curr + 3);

			indices.push_back(next + 2);
			indices.push_back(next + 3);
			indices.push_back(curr + 3);
		}

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
