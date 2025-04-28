/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitivePlane.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:30:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 10:25:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <algorithm> // For std::max
#include <memory>
#include <vector>

namespace Engine {

	std::unique_ptr<Mesh> Primitives::CreatePlane(unsigned int subdivisions) {
		std::vector<Vertex> verts;
		std::vector<unsigned int> idx;
		unsigned int div = std::max(1u, subdivisions);

		verts.reserve((div + 1) * (div + 1));
		idx.reserve(div * div * 6);

		glm::vec3 normal(0.0f, 1.0f, 0.0f);
		glm::vec3 tangent(1.0f, 0.0f, 0.0f);   // Along +X
		glm::vec3 bitangent(0.0f, 0.0f, 1.0f); // Along +Z

		for (unsigned int z = 0; z <= div; ++z) {
			for (unsigned int x = 0; x <= div; ++x) {
				float fx = (float(x) / div - 0.5f);
				float fz = (float(z) / div - 0.5f);
				glm::vec3 pos(fx, 0.0f, fz);
				glm::vec2 uv(float(x) / div, float(z) / div);
				verts.push_back({pos, normal, uv, tangent, bitangent});
			}
		}
		for (unsigned int z = 0; z < div; ++z) {
			for (unsigned int x = 0; x < div; ++x) {
				unsigned int i0 = z * (div + 1) + x;
				unsigned int i1 = i0 + 1;
				unsigned int i2 = i0 + (div + 1);
				unsigned int i3 = i2 + 1;
				idx.insert(idx.end(), {i0, i2, i1, i1, i2, i3});
			}
		}
		return std::make_unique<Mesh>(verts, idx);
	}

} // namespace Engine
