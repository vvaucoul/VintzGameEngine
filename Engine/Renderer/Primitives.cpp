/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Primitives.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:27:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:04:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Primitives.h"
#include <cmath>
#include <memory> // For std::make_unique
#include <vector>
#include <map> // For cube vertex deduplication logic

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine {

	// Helper function to calculate tangent/bitangent for a triangle
	// Note: This is a simplified approach. Robust calculation often involves averaging over adjacent triangles.
	void CalculateTangents(Vertex &v0, Vertex &v1, Vertex &v2) {
		glm::vec3 edge1 = v1.Position - v0.Position;
		glm::vec3 edge2 = v2.Position - v0.Position;
		glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
		glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		if (std::isinf(f) || std::isnan(f)) {
			// Handle degenerate UVs (e.g., use arbitrary orthogonal vectors)
			glm::vec3 normal = v0.Normal; // Assuming normal is already calculated
			glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);
			if (glm::abs(glm::dot(normal, tangent)) > 0.99f) {
				tangent = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			tangent = glm::normalize(tangent - normal * glm::dot(tangent, normal));
			glm::vec3 bitangent = glm::cross(normal, tangent);

			v0.Tangent = v1.Tangent = v2.Tangent = tangent;
			v0.Bitangent = v1.Bitangent = v2.Bitangent = bitangent;
			return;
		}

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		// Orthogonalize tangent with respect to normal (Gram-Schmidt)
		// tangent = glm::normalize(tangent - v0.Normal * glm::dot(tangent, v0.Normal));
		// bitangent = glm::cross(v0.Normal, tangent); // Recalculate bitangent

		// Assign calculated tangents/bitangents (simple assignment, averaging is better)
		v0.Tangent = v1.Tangent = v2.Tangent = tangent;
		v0.Bitangent = v1.Bitangent = v2.Bitangent = bitangent;
	}

	std::unique_ptr<Mesh> Primitives::CreateCube() {
		// Cube requires vertex duplication for correct per-face normals/tangents/uvs
		std::vector<Vertex> vertices = {
			// Back face (-Z) Tangent: +X, Bitangent: -Y
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

			// Front face (+Z) Tangent: -X, Bitangent: -Y
			{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

			// Left face (-X) Tangent: -Z, Bitangent: -Y
			{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}},
			{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}},

			// Right face (+X) Tangent: +Z, Bitangent: -Y
			{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

			// Bottom face (-Y) Tangent: +X, Bitangent: +Z
			{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

			// Top face (+Y) Tangent: +X, Bitangent: -Z
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}};

		std::vector<unsigned int> indices = {
			0, 1, 2, 2, 3, 0,		 // Back
			4, 6, 5, 6, 4, 7,		 // Front
			8, 9, 10, 10, 11, 8,	 // Left
			12, 14, 13, 14, 12, 15, // Right
			16, 17, 18, 18, 19, 16, // Bottom
			20, 22, 21, 22, 20, 23	 // Top
		};

		return std::make_unique<Mesh>(vertices, indices);
	}

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

	std::unique_ptr<Mesh> Primitives::CreateSphere(unsigned int sectorCount, unsigned int stackCount) {
		std::vector<Vertex> verts;
		std::vector<unsigned int> idx;
		float radius = 0.5f;

		verts.reserve((stackCount + 1) * (sectorCount + 1));
		idx.reserve(stackCount * sectorCount * 6);

		for (unsigned int i = 0; i <= stackCount; ++i) {
			float stackAngle = M_PI / 2.0f - (float)i / stackCount * M_PI; // From +pi/2 to -pi/2
			float xy		 = radius * cosf(stackAngle);
			float y			 = radius * sinf(stackAngle);

			for (unsigned int j = 0; j <= sectorCount; ++j) {
				float sectorAngle = (float)j / sectorCount * 2.0f * M_PI; // From 0 to 2pi
				float x			  = xy * cosf(sectorAngle);
				float z			  = xy * sinf(sectorAngle);

				glm::vec3 pos(x, y, z);
				glm::vec3 normal = glm::normalize(pos);
				glm::vec2 uv((float)j / sectorCount, (float)i / stackCount);

				// Calculate tangent and bitangent
				// Tangent is derivative with respect to sector angle (phi)
				glm::vec3 tangent(-radius * sinf(stackAngle) * sinf(sectorAngle), 0, radius * sinf(stackAngle) * cosf(sectorAngle));
				// Bitangent is derivative with respect to stack angle (theta)
				glm::vec3 bitangent(radius * cosf(stackAngle) * cosf(sectorAngle), -radius * sinf(stackAngle), radius * cosf(stackAngle) * sinf(sectorAngle));

				// Handle poles where tangent might be zero or undefined
				if (i == 0 || i == stackCount) {
					tangent = glm::vec3(1.0f, 0.0f, 0.0f); // Assign arbitrary tangent at poles
					bitangent = glm::cross(normal, tangent);
				} else {
					tangent = glm::normalize(tangent);
					bitangent = glm::normalize(bitangent);
					// Optional: Re-orthogonalize using cross product
					// tangent = glm::normalize(glm::cross(bitangent, normal));
					// bitangent = glm::normalize(glm::cross(normal, tangent));
				}

				verts.push_back({pos, normal, uv, tangent, bitangent});
			}
		}

		// Indices remain the same logic
		for (unsigned int i = 0; i < stackCount; ++i) {
			unsigned int k1 = i * (sectorCount + 1);
			unsigned int k2 = k1 + sectorCount + 1;
			for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
				// 2 triangles per sector excluding first and last stacks
				if (i != 0) {
					idx.push_back(k1);
					idx.push_back(k2);
					idx.push_back(k1 + 1);
				}
				if (i != (stackCount - 1)) {
					idx.push_back(k1 + 1);
					idx.push_back(k2);
					idx.push_back(k2 + 1);
				}
			}
		}
		return std::make_unique<Mesh>(verts, idx);
	}

	std::unique_ptr<Mesh> Primitives::CreateCylinder(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		float halfHeight = height * 0.5f;

		// Use a map to help deduplicate vertices for sides later if needed, though direct generation is fine here.
		// std::map<unsigned int, glm::vec3> tangentMap;
		// std::map<unsigned int, glm::vec3> bitangentMap;

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
			glm::vec3 capTangent = glm::normalize(glm::vec3(-z, 0.0f, x)); // Tangent along circle
			glm::vec3 capBitangentTop = glm::cross(topNormal, capTangent);
			glm::vec3 capBitangentBottom = glm::cross(bottomNormal, capTangent);
			glm::vec2 capUV((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
			vertices.push_back({{x, halfHeight, z}, topNormal, capUV, capTangent, capBitangentTop});		// Top cap ring
			vertices.push_back({{x, -halfHeight, z}, bottomNormal, capUV, capTangent, capBitangentBottom}); // Bottom cap ring

			// Side vertices
			glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));
			glm::vec3 sideTangent(0.0f, 1.0f, 0.0f); // Along the height
			glm::vec3 sideBitangent = glm::normalize(glm::cross(sideNormal, sideTangent));
			vertices.push_back({{x, halfHeight, z}, sideNormal, {u, 1.0f}, sideTangent, sideBitangent}); // Top side ring
			vertices.push_back({{x, -halfHeight, z}, sideNormal, {u, 0.0f}, sideTangent, sideBitangent}); // Bottom side ring
		}

		// Generate indices
		for (unsigned int i = 0; i < sectorCount; ++i) {
			unsigned int current = baseVertexIndex + i * 4;
			unsigned int next	 = baseVertexIndex + ((i + 1) % (sectorCount + 1)) * 4; // Wrap around using modulo? No, use i+1 directly as we generate sectorCount+1 vertices

			// Top cap triangle
			indices.push_back(topCenterIndex);
			indices.push_back(next + 0); // Next top cap vertex
			indices.push_back(current + 0); // Current top cap vertex

			// Bottom cap triangle
			indices.push_back(bottomCenterIndex);
			indices.push_back(current + 1); // Current bottom cap vertex
			indices.push_back(next + 1); // Next bottom cap vertex

			// Side quad (two triangles)
			indices.push_back(current + 2); // Current top side vertex
			indices.push_back(next + 2);	// Next top side vertex
			indices.push_back(current + 3); // Current bottom side vertex

			indices.push_back(next + 2);	// Next top side vertex
			indices.push_back(next + 3);	// Next bottom side vertex
			indices.push_back(current + 3); // Current bottom side vertex
		}

		// Tangent calculation (simple assignment done inline, could refine with averaging)

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Primitives::CreateCone(float height, float radius, unsigned int sectorCount) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		float halfHeight = height * 0.5f;

		// Apex vertex
		glm::vec3 apexPos(0.0f, halfHeight, 0.0f);
		glm::vec3 apexNormal(0.0f, 1.0f, 0.0f); // Normal points up, might need averaging later
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
			glm::vec3 baseRingTangent = glm::normalize(glm::vec3(-z, 0.0f, x)); // Tangent along circle
			glm::vec3 baseRingBitangent = glm::cross(baseNormal, baseRingTangent);
			glm::vec2 baseUV((x / radius + 1.0f) * 0.5f, (z / radius + 1.0f) * 0.5f);
			vertices.push_back({basePos, baseNormal, baseUV, baseRingTangent, baseRingBitangent}); // Base ring vertex

			// Side vertex (calculate normal for the slanted surface)
			glm::vec3 sidePos = basePos; // Side vertex shares position with base vertex
			glm::vec3 edge	  = glm::normalize(apexPos - sidePos);
			glm::vec3 sideNormal = glm::normalize(glm::cross(baseRingTangent, edge)); // Normal perpendicular to edge and tangent
			glm::vec3 sideTangent = baseRingTangent; // Use the same tangent as the base ring
			glm::vec3 sideBitangent = glm::normalize(glm::cross(sideNormal, sideTangent));
			glm::vec2 sideUV(u, 0.0f);
			vertices.push_back({sidePos, sideNormal, sideUV, sideTangent, sideBitangent}); // Side ring vertex
		}

		// Generate indices
		for (unsigned int i = 0; i < sectorCount; ++i) {
			unsigned int current = baseVertexIndex + i * 2;
			unsigned int next	 = baseVertexIndex + ((i + 1) % (sectorCount + 1)) * 2; // Wrap around? No, use i+1

			// Base triangle
			indices.push_back(baseCenterIndex);
			indices.push_back(next + 0); // Next base vertex
			indices.push_back(current + 0); // Current base vertex

			// Side triangle
			indices.push_back(apexIndex);
			indices.push_back(next + 1); // Next side vertex
			indices.push_back(current + 1); // Current side vertex
		}

		// Refine normals/tangents at apex? For cones, the normal/tangent is discontinuous at the apex.
		// Averaging might smooth it undesirably. Using the calculated side normals is common.

		return std::make_unique<Mesh>(vertices, indices);
	}

	std::unique_ptr<Mesh> Primitives::CreateTorus(float mainRadius, float tubeRadius, unsigned int mainSegments, unsigned int tubeSegments) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		vertices.reserve((mainSegments + 1) * (tubeSegments + 1));
		indices.reserve(mainSegments * tubeSegments * 6);

		for (unsigned int i = 0; i <= mainSegments; ++i) {
			float mainAngle = (float)i / mainSegments * 2.0f * M_PI;
			float cosMain = cosf(mainAngle);
			float sinMain = sinf(mainAngle);
			glm::vec3 mainCirclePos(cosMain * mainRadius, 0.0f, sinMain * mainRadius);

			for (unsigned int j = 0; j <= tubeSegments; ++j) {
				float tubeAngle = (float)j / tubeSegments * 2.0f * M_PI;
				float cosTube = cosf(tubeAngle);
				float sinTube = sinf(tubeAngle);

				// Calculate position
				float x = (mainRadius + tubeRadius * cosTube) * cosMain;
				float y = tubeRadius * sinTube;
				float z = (mainRadius + tubeRadius * cosTube) * sinMain;
				glm::vec3 pos(x, y, z);

				// Calculate normal (points from tube center outwards)
				glm::vec3 tubeCenter = mainCirclePos;
				glm::vec3 normal = glm::normalize(pos - tubeCenter);

				// Calculate UVs
				float u = (float)i / mainSegments;
				float v = (float)j / tubeSegments;

				// Calculate Tangent (derivative wrt mainAngle) and Bitangent (derivative wrt tubeAngle)
				glm::vec3 tangent(-(mainRadius + tubeRadius * cosTube) * sinMain, 0, (mainRadius + tubeRadius * cosTube) * cosMain);
				glm::vec3 bitangent(-tubeRadius * sinTube * cosMain, tubeRadius * cosTube, -tubeRadius * sinTube * sinMain);

				tangent = glm::normalize(tangent);
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
