/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitivePlane.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:57:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Geometry/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace Engine {

	// Generates a subdivided plane mesh centered at the origin, lying on the XZ plane.
	// - subdivisions: number of quads per axis (minimum 1)
	std::unique_ptr<Mesh> Primitives::CreatePlane(unsigned int subdivisions) {
		const unsigned int div		  = std::max(1u, subdivisions);
		const unsigned int vertCount  = (div + 1) * (div + 1);
		const unsigned int indexCount = div * div * 6;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		vertices.reserve(vertCount);
		indices.reserve(indexCount);

		const glm::vec3 normal(0.0f, 1.0f, 0.0f);
		const glm::vec3 tangent(1.0f, 0.0f, 0.0f);
		const glm::vec3 bitangent(0.0f, 0.0f, 1.0f);

		// Generate vertices
		for (unsigned int z = 0; z <= div; ++z) {
			for (unsigned int x = 0; x <= div; ++x) {
				float fx = float(x) / div - 0.5f;
				float fz = float(z) / div - 0.5f;
				glm::vec3 position(fx, 0.0f, fz);
				glm::vec2 uv(float(x) / div, float(z) / div);
				vertices.push_back({position, normal, uv, tangent, bitangent});
			}
		}

		// Generate indices (two triangles per quad)
		for (unsigned int z = 0; z < div; ++z) {
			for (unsigned int x = 0; x < div; ++x) {
				unsigned int i0 = z * (div + 1) + x;
				unsigned int i1 = i0 + 1;
				unsigned int i2 = i0 + (div + 1);
				unsigned int i3 = i2 + 1;
				indices.push_back(i0);
				indices.push_back(i2);
				indices.push_back(i1);
				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
			}
		}

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
