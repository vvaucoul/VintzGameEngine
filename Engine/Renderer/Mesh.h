/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:26:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 22:47:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Engine {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	class Mesh {
	public:
		Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
		~Mesh();

		// Draw no longer needs the shader reference, assumes it's bound externally
		void Draw() const;

		// Getters for vertices and indices
		const std::vector<Vertex> &GetVertices() const { return m_Vertices; }
		const std::vector<unsigned int> &GetIndices() const { return m_Indices; }

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		void SetupMesh();

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
	};

} // namespace Engine