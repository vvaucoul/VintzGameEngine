/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:15:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 00:49:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Engine {

	class Mesh;
	class Texture;
	class Shader;
	class MaterialPBR; // Forward declaration

	/**
	 * @brief Represents a 3D model loaded from file (via Assimp).
	 *        Manages sub-meshes and their materials.
	 */
	class Model {
	public:
		/**
		 * @brief Loads a model from file (supports formats via Assimp).
		 * @param path Path to the model file.
		 */
		Model(const std::string &path);

		/**
		 * @brief Destructor. Cleans up loaded resources.
		 */
		~Model();

		/**
		 * @brief Draws all sub-meshes with their materials using the given shader.
		 * @param shader Shader to use for rendering.
		 */
		void Draw(Shader &shader) const;

		/**
		 * @brief Draws only geometry (no material handling).
		 *        Useful for depth or shadow passes.
		 * @param shader Shader to use for rendering.
		 */
		void DrawGeometry(Shader &shader) const;

	private:
		/**
		 * @brief Represents a sub-mesh and its material.
		 */
		struct SubMesh {
			std::unique_ptr<Mesh> mesh;			   ///< Mesh geometry
			std::shared_ptr<MaterialPBR> material; ///< Material (may be nullptr)
		};

		std::vector<SubMesh> m_SubMeshes;								  ///< All sub-meshes in the model
		std::string m_Directory;										  ///< Directory of the model file
		std::map<std::string, std::shared_ptr<Texture>> m_LoadedTextures; ///< Texture cache

		/// Loads the model from file.
		void LoadModel(const std::string &path);

		/// Recursively processes Assimp nodes.
		void ProcessNode(aiNode *node, const aiScene *scene);

		/// Processes an Assimp mesh and returns a SubMesh.
		SubMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

		/// Loads a texture from a material, using cache if possible.
		std::shared_ptr<Texture> LoadMaterialTexture(aiMaterial *mat, aiTextureType type);
	};

}
