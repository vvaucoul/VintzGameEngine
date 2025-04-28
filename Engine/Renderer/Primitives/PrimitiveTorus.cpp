/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveTorus.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:05 by vvaucoul          #+#    #+#             */
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

	std::unique_ptr<Mesh> Primitives::CreateTorus(float mainRadius, float tubeRadius, unsigned int mainSegments, unsigned int tubeSegments) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		vertices.reserve((mainSegments + 1) * (tubeSegments + 1));
		indices.reserve(mainSegments * tubeSegments * 6);

		for (unsigned int i = 0; i <= mainSegments; ++i) {
			float mainAngle = (float)i / mainSegments * 2.0f * M_PI;
			float cosMain	= cosf(mainAngle);
			float sinMain	= sinf(mainAngle);
			glm::vec3 mainCirclePos(cosMain * mainRadius, 0.0f, sinMain * mainRadius);

			for (unsigned int j = 0; j <= tubeSegments; ++j) {
				float tubeAngle = (float)j / tubeSegments * 2.0f * M_PI;
				float cosTube	= cosf(tubeAngle);
				float sinTube	= sinf(tubeAngle);

				// Calculate position
				float x = (mainRadius + tubeRadius * cosTube) * cosMain;
				float y = tubeRadius * sinTube;
				float z = (mainRadius + tubeRadius * cosTube) * sinMain;
				glm::vec3 pos(x, y, z);

				// Calculate normal (points from tube center outwards)
				glm::vec3 tubeCenter = mainCirclePos;
				glm::vec3 normal	 = glm::normalize(pos - tubeCenter);

				// Calculate UVs
				float u = (float)i / mainSegments;
				float v = (float)j / tubeSegments;

				// Calculate Tangent (derivative wrt mainAngle) and Bitangent (derivative wrt tubeAngle)
				glm::vec3 tangent(-(mainRadius + tubeRadius * cosTube) * sinMain, 0, (mainRadius + tubeRadius * cosTube) * cosMain);
				glm::vec3 bitangent(-tubeRadius * sinTube * cosMain, tubeRadius * cosTube, -tubeRadius * sinTube * sinMain);

				tangent	  = glm::normalize(tangent);
				bitangent = glm::normalize(bitangent);
				// Optional: Re-orthogonalize
				// tangent = glm::normalize(glm::cross(bitangent, normal));
				// bitangent = glm::normalize(glm::cross(normal, tangent));

				vertices.push_back({pos, normal, {u, v}, tangent, bitangent});
			}
		}

		// Indices remain the same
		for (unsigned int i = 0; i < mainSegments; ++i) {
			for (unsigned int j = 0; j < tubeSegments; ++j) {
				unsigned int i0 = i * (tubeSegments + 1) + j;
				unsigned int i1 = i0 + 1;
				unsigned int i2 = (i + 1) * (tubeSegments + 1) + j; // Next main segment, same tube segment
				unsigned int i3 = i2 + 1;							// Next main segment, next tube segment

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
