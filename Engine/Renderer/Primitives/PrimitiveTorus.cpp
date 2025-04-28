/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveTorus.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:12:24 by vvaucoul         ###   ########.fr       */
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

	// Generates a torus mesh centered at the origin, lying on the XZ plane.
	// mainRadius: distance from center to tube center
	// tubeRadius: radius of the tube
	// mainSegments: number of segments around the main ring
	// tubeSegments: number of segments around the tube
	std::unique_ptr<Mesh> Primitives::CreateTorus(float mainRadius, float tubeRadius, unsigned int mainSegments, unsigned int tubeSegments) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		vertices.reserve((mainSegments + 1) * (tubeSegments + 1));
		indices.reserve(mainSegments * tubeSegments * 6);

		for (unsigned int i = 0; i <= mainSegments; ++i) {
			float theta	   = float(i) / mainSegments * 2.0f * M_PI;
			float cosTheta = cosf(theta);
			float sinTheta = sinf(theta);

			// Center of tube circle for this main segment
			glm::vec3 circleCenter = {cosTheta * mainRadius, 0.0f, sinTheta * mainRadius};

			for (unsigned int j = 0; j <= tubeSegments; ++j) {
				float phi	 = float(j) / tubeSegments * 2.0f * M_PI;
				float cosPhi = cosf(phi);
				float sinPhi = sinf(phi);

				// Position of vertex
				float x = (mainRadius + tubeRadius * cosPhi) * cosTheta;
				float y = tubeRadius * sinPhi;
				float z = (mainRadius + tubeRadius * cosPhi) * sinTheta;
				glm::vec3 pos(x, y, z);

				// Normal: from tube center to vertex
				glm::vec3 normal = glm::normalize(pos - circleCenter);

				// Texture coordinates
				glm::vec2 uv(float(i) / mainSegments, float(j) / tubeSegments);

				// Tangent: direction of increasing theta (main ring)
				glm::vec3 tangent = glm::normalize(glm::vec3(
					-sinTheta * (mainRadius + tubeRadius * cosPhi),
					0.0f,
					cosTheta * (mainRadius + tubeRadius * cosPhi)));

				// Bitangent: direction of increasing phi (tube ring)
				glm::vec3 bitangent = glm::normalize(glm::vec3(
					-cosTheta * tubeRadius * sinPhi,
					tubeRadius * cosPhi,
					-sinTheta * tubeRadius * sinPhi));

				vertices.push_back({pos, normal, uv, tangent, bitangent});
			}
		}

		// Generate indices for triangles
		for (unsigned int i = 0; i < mainSegments; ++i) {
			for (unsigned int j = 0; j < tubeSegments; ++j) {
				unsigned int i0 = i * (tubeSegments + 1) + j;
				unsigned int i1 = i0 + 1;
				unsigned int i2 = (i + 1) * (tubeSegments + 1) + j;
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
