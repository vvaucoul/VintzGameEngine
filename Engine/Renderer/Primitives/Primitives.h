/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Primitives.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:27:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 15:12:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderer/Mesh.h"
#include <memory>

/**
 * @file Primitives.h
 * @brief Declarations for procedural mesh generation utilities.
 */

namespace Engine {

	/**
	 * @class Primitives
	 * @brief Utility class for generating common geometric meshes.
	 *
	 * All methods return a unique_ptr to a Mesh, ready for use with StaticMeshComponent.
	 * Meshes are centered at the origin and sized to unit scale unless otherwise specified.
	 */
	class Primitives {
	public:
		/**
		 * @brief Creates a unit cube mesh centered at the origin.
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateCube();

		/**
		 * @brief Creates a plane mesh on the XZ plane, centered at the origin.
		 * @param subdivisions Number of subdivisions per axis (minimum 1).
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreatePlane(unsigned int subdivisions = 1);

		/**
		 * @brief Creates a UV sphere mesh centered at the origin.
		 * @param sectorCount Number of longitudinal segments (minimum 3).
		 * @param stackCount Number of latitudinal segments (minimum 2).
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateSphere(unsigned int sectorCount = 36, unsigned int stackCount = 18);

		/**
		 * @brief Creates a cylinder mesh aligned along the Y axis, centered at the origin.
		 * @param height Height of the cylinder.
		 * @param radius Radius of the cylinder.
		 * @param sectorCount Number of radial segments (minimum 3).
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateCylinder(float height = 1.0f, float radius = 0.5f, unsigned int sectorCount = 36);

		/**
		 * @brief Creates a cone mesh aligned along the Y axis, centered at the origin.
		 * @param height Height of the cone.
		 * @param radius Base radius of the cone.
		 * @param sectorCount Number of radial segments (minimum 3).
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateCone(float height = 1.0f, float radius = 0.5f, unsigned int sectorCount = 36);

		/**
		 * @brief Creates a torus mesh centered at the origin, lying on the XZ plane.
		 * @param mainRadius Radius from the center to the middle of the tube.
		 * @param tubeRadius Radius of the tube.
		 * @param mainSegments Number of segments around the main ring (minimum 3).
		 * @param tubeSegments Number of segments around the tube (minimum 3).
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateTorus(float mainRadius = 1.0f, float tubeRadius = 0.2f, unsigned int mainSegments = 36, unsigned int tubeSegments = 18);

		/**
		 * @brief Creates a unit quad mesh on the XY plane, centered at the origin.
		 * @return Unique pointer to the generated Mesh.
		 */
		static std::unique_ptr<Mesh> CreateQuad();
	};

} // namespace Engine
