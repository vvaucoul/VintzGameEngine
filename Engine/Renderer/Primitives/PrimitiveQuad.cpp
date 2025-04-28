/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrimitiveQuad.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:45:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:57:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer/Geometry/Mesh.h"
#include "Renderer/Primitives/Primitives.h"
#include <memory>
#include <vector>

namespace Engine {

	// Generates a unit quad centered at the origin, lying on the XY plane, facing +Z.
	// Vertex order: bottom-left, bottom-right, top-right, top-left.
	std::unique_ptr<Mesh> Primitives::CreateQuad() {
		std::vector<Vertex> vertices = {
			// Position                Normal              TexCoord    Tangent             Bitangent
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom Left
			{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom Right
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},	  // Top Right
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	  // Top Left
		};

		// Indices for two triangles: (0, 1, 2) and (2, 3, 0)
		std::vector<unsigned int> indices = {
			0, 1, 2, 2, 3, 0};

		return std::make_unique<Mesh>(vertices, indices);
	}

} // namespace Engine
