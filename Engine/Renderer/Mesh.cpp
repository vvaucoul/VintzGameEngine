/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 12:26:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 23:49:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <glad/glad.h>
#include <iostream> // For debugging

namespace Engine {

	Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
		: m_Vertices(vertices), m_Indices(indices) {
		// Debug print for mesh creation
		// std::cout << "  Mesh::Mesh - Creating mesh with " << m_Vertices.size() << " vertices and " << m_Indices.size() << " indices." << std::endl;
		if (m_Vertices.empty() || m_Indices.empty()) {
			std::cerr << "Warning: Creating Mesh with empty vertices or indices!" << std::endl;
		}
		SetupMesh();
	}

	Mesh::~Mesh() {
		// std::cout << "  Mesh::~Mesh - Destroying mesh." << std::endl; // Optional: Check destruction
	}

	void Mesh::SetupMesh() {
		// std::cout << "    Mesh::SetupMesh - Setting up VAO/VBO/IBO..." << std::endl; // Debug print
		m_VertexArray = std::make_shared<VertexArray>();
		m_VertexArray->Bind();

		m_VertexBuffer = std::make_shared<VertexBuffer>((float *)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		// Position attribute (location 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));

		// Normal attribute (location 1)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

		// Texture Coordinate attribute (location 2)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

		// Tangent attribute (location 3)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

		// Bitangent attribute (location 4)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));

		m_IndexBuffer = std::make_shared<IndexBuffer>(m_Indices.data(), m_Indices.size());

		m_VertexArray->Unbind();
		// std::cout << "    Mesh::SetupMesh - Setup complete." << std::endl; // Debug print
	}

	void Mesh::Draw() const {
		if (m_Indices.empty()) {
			// std::cerr << "Warning: Mesh::Draw called with 0 indices." << std::endl; // Debug print
			return;
		}
		// std::cout << "    Mesh::Draw - Binding VAO and drawing " << m_Indices.size() << " indices." << std::endl; // Debug print
		m_VertexArray->Bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
		m_VertexArray->Unbind();
	}

} // namespace Engine
