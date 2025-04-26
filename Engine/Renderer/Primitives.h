/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Primitives.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:27:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:04:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.h"
#include <memory> // For std::unique_ptr

/**
 * @namespace Engine
 * @brief Contains core classes and functionality for the game engine, including rendering primitives.
 */
namespace Engine {

	class Primitives {
	public:
		// Use unique_ptr to manage Mesh lifetime and avoid raw new/delete
		static std::unique_ptr<Mesh> CreateCube();
		static std::unique_ptr<Mesh> CreatePlane(unsigned int subdivisions = 1);
		static std::unique_ptr<Mesh> CreateSphere(unsigned int sectorCount = 36, unsigned int stackCount = 18);
		static std::unique_ptr<Mesh> CreateCylinder(float height = 1.0f, float radius = 0.5f, unsigned int sectorCount = 36);
		static std::unique_ptr<Mesh> CreateCone(float height = 1.0f, float radius = 0.5f, unsigned int sectorCount = 36);
		static std::unique_ptr<Mesh> CreateTorus(float mainRadius = 1.0f, float tubeRadius = 0.2f, unsigned int mainSegments = 36, unsigned int tubeSegments = 18);
	};

} // namespace Engine
