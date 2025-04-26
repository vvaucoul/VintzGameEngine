/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:15:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/26 20:47:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map> // Include for std::map

namespace Engine {
	class Mesh;
	class Texture;
	class Shader;
	class MaterialPBR; // Forward declare MaterialPBR

	class Model {
	public:
		// Constructeur : charge le .obj via Assimp
		Model(const std::string &path);
		~Model();

		// Dessine tous les sous-meshes avec leurs textures
		void Draw(Shader &shader) const;
		void DrawGeometry(Shader &shader) const; // Draw without handling materials

	private:
		struct SubMesh {
			std::unique_ptr<Mesh> mesh;
			std::shared_ptr<MaterialPBR> material; // Use MaterialPBR pointer
		};
		std::vector<SubMesh> m_SubMeshes;
		std::string m_Directory; // Store the directory of the model file
		// Texture cache
		std::map<std::string, std::shared_ptr<Texture>> m_LoadedTextures;

		void LoadModel(const std::string &path);
		void ProcessNode(aiNode *node, const aiScene *scene);
		SubMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::shared_ptr<Texture> LoadMaterialTexture(aiMaterial *mat, aiTextureType type);
	};
}
