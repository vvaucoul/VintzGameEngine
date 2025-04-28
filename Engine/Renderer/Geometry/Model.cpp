/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:49:21 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 00:55:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model.h"
#include "Mesh.h"
#include "Renderer/Materials/MaterialPBR.h"
#include "Renderer/Shaders/Shader.h"
#include "Renderer/Textures/Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <iostream>
#include <map>

namespace Engine {

	/**
	 * @brief Construct a Model by loading from file.
	 * @param path Path to the model file.
	 */
	Model::Model(const std::string &path) {
		LoadModel(path);
	}

	Model::~Model() = default;

	/**
	 * @brief Draws all sub-meshes with their materials using the given shader.
	 * @param shader Shader to use for rendering.
	 */
	void Model::Draw(Shader &shader) const {
		for (const auto &sub : m_SubMeshes) {
			// Bind material textures and set uniforms if material exists
			if (sub.material) {
				// Albedo map or color
				if (sub.material->hasAlbedoMap && sub.material->albedoMap) {
					sub.material->albedoMap->Bind(0);
					shader.SetUniformInt("u_AlbedoMap", 0);	   // Use u_ prefix
					shader.SetUniformInt("u_HasAlbedoMap", 1); // Use u_ prefix
				} else {
					shader.SetUniformInt("u_HasAlbedoMap", 0);						   // Use u_ prefix
					shader.SetUniformVec3("u_AlbedoColor", sub.material->albedoColor); // Use u_ prefix
				}
				// Normal map
				if (sub.material->hasNormalMap && sub.material->normalMap) {
					sub.material->normalMap->Bind(1);
					shader.SetUniformInt("u_NormalMap", 1);	   // Use u_ prefix
					shader.SetUniformInt("u_HasNormalMap", 1); // Use u_ prefix
				} else {
					shader.SetUniformInt("u_HasNormalMap", 0); // Use u_ prefix
				}
				// Ambient Occlusion map
				if (sub.material->hasAOMap && sub.material->aoMap) {
					// Bind AO map to texture unit 3 (as decided previously)
					sub.material->aoMap->Bind(3);
					shader.SetUniformInt("u_AOMap", 3);	   // Use u_ prefix and correct unit
					shader.SetUniformInt("u_HasAOMap", 1); // Use u_ prefix and correct name
				} else {
					shader.SetUniformInt("u_HasAOMap", 0); // Use u_ prefix and correct name
				}
				// PBR factors
				shader.SetUniformFloat("u_Metallic", sub.material->metallic);	// Use u_ prefix
				shader.SetUniformFloat("u_Roughness", sub.material->roughness); // Use u_ prefix
				shader.SetUniformFloat("u_AO", sub.material->ao);				// Use u_ prefix
			} else {
				// Fallback to default material values
				shader.SetUniformInt("u_HasAlbedoMap", 0);					// Use u_ prefix
				shader.SetUniformVec3("u_AlbedoColor", {0.8f, 0.8f, 0.8f}); // Use u_ prefix
				shader.SetUniformInt("u_HasNormalMap", 0);					// Use u_ prefix
				shader.SetUniformInt("u_HasAOMap", 0);						// Use u_ prefix and correct name
				shader.SetUniformFloat("u_Metallic", 0.1f);					// Use u_ prefix
				shader.SetUniformFloat("u_Roughness", 0.8f);				// Use u_ prefix
				shader.SetUniformFloat("u_AO", 1.0f);						// Use u_ prefix
			}
			// Draw mesh geometry
			sub.mesh->Draw();
		}
	}

	/**
	 * @brief Draws only geometry (no material uniforms).
	 * @param shader Shader to use for rendering.
	 */
	void Model::DrawGeometry([[maybe_unused]] Shader &shader) const {
		for (const auto &sub : m_SubMeshes) {
			sub.mesh->Draw();
		}
	}

	/**
	 * @brief Loads the model from file using Assimp.
	 * @param path Path to the model file.
	 */
	void Model::LoadModel(const std::string &path) {
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path,
												 aiProcess_Triangulate |
													 aiProcess_FlipUVs |
													 aiProcess_CalcTangentSpace |
													 aiProcess_GenSmoothNormals);

		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			std::cerr << "Assimp load error: " << importer.GetErrorString() << "\n";
			return;
		}
		// Extract directory for texture loading
		m_Directory = path.substr(0, path.find_last_of('/'));
		if (m_Directory == path) {
			m_Directory = ".";
		}
		ProcessNode(scene->mRootNode, scene);
	}

	/**
	 * @brief Recursively process Assimp nodes and meshes.
	 * @param node Current node.
	 * @param scene Assimp scene.
	 */
	void Model::ProcessNode(aiNode *node, const aiScene *scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			m_SubMeshes.push_back(ProcessMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			ProcessNode(node->mChildren[i], scene);
	}

	/**
	 * @brief Converts an Assimp mesh to engine Mesh and loads its material.
	 * @param mesh Assimp mesh.
	 * @param scene Assimp scene.
	 * @return SubMesh containing geometry and material.
	 */
	Model::SubMesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<Vertex> verts;
		std::vector<unsigned int> indices;
		std::shared_ptr<MaterialPBR> material = nullptr;

		verts.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex v;
			v.Position	= {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
			v.Normal	= mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f, 1.0f, 0.0f);
			v.TexCoords = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
			v.Tangent	= mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z) : glm::vec3(1.0f, 0.0f, 0.0f);
			v.Bitangent = mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z) : glm::vec3(0.0f, 0.0f, 1.0f);
			verts.push_back(v);
		}

		indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices == 3) {
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}
		}

		// Load material if present
		if (mesh->mMaterialIndex < scene->mNumMaterials) {
			aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];
			material		  = std::make_shared<MaterialPBR>();

			// Load textures
			material->albedoMap = LoadMaterialTexture(aiMat, aiTextureType_DIFFUSE);
			material->normalMap = LoadMaterialTexture(aiMat, aiTextureType_NORMALS);
			material->aoMap		= LoadMaterialTexture(aiMat, aiTextureType_AMBIENT_OCCLUSION);

			// Set flags
			material->hasAlbedoMap = (material->albedoMap != nullptr);
			material->hasNormalMap = (material->normalMap != nullptr);
			material->hasAOMap	   = (material->aoMap != nullptr);

			// Load color and PBR factors
			aiColor4D color;
			if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
				material->albedoColor = {color.r, color.g, color.b};
			}
			float factor = 0.0f;
			if (aiGetMaterialFloat(aiMat, AI_MATKEY_METALLIC_FACTOR, &factor) == AI_SUCCESS) {
				material->metallic = factor;
			} else if (aiGetMaterialFloat(aiMat, "$mat.metallicFactor", 0, 0, &factor) == AI_SUCCESS) {
				material->metallic = factor;
			} else {
				material->metallic = 0.1f;
			}
			if (aiGetMaterialFloat(aiMat, AI_MATKEY_ROUGHNESS_FACTOR, &factor) == AI_SUCCESS) {
				material->roughness = factor;
			} else if (aiGetMaterialFloat(aiMat, "$mat.roughnessFactor", 0, 0, &factor) == AI_SUCCESS) {
				material->roughness = factor;
			} else {
				material->roughness = 0.8f;
			}
			material->ao = 1.0f;
		}

		auto meshPtr = std::make_unique<Mesh>(verts, indices);
		return {std::move(meshPtr), material};
	}

	/**
	 * @brief Loads a texture from a material, using cache if possible.
	 * @param mat Assimp material.
	 * @param type Texture type.
	 * @return Shared pointer to loaded Texture, or nullptr.
	 */
	std::shared_ptr<Texture> Model::LoadMaterialTexture(aiMaterial *mat, aiTextureType type) {
		if (mat->GetTextureCount(type) > 0) {
			aiString str;
			mat->GetTexture(type, 0, &str);
			std::string textureFile = str.C_Str();

			// Ignore embedded or invalid texture names
			if (textureFile.empty() || textureFile[0] == '*') {
				return nullptr;
			}

			std::string texturePath = m_Directory + "/" + textureFile;

			// Use cached texture if already loaded
			if (m_LoadedTextures.count(texturePath)) {
				return m_LoadedTextures[texturePath];
			}

			// Check file existence
			std::ifstream f(texturePath.c_str());
			if (!f.good()) {
				std::cerr << "Warning: Texture file not found: " << texturePath << std::endl;
				texturePath = textureFile;
				std::ifstream f_base(texturePath.c_str());
				if (!f_base.good()) {
					std::cerr << "Warning: Texture file not found in base directory either: " << texturePath << std::endl;
					return nullptr;
				}
			}
			f.close();

			// Load and cache texture
			auto texture = std::make_shared<Texture>(texturePath);
			if (texture->GetID() != 0) {
				m_LoadedTextures[texturePath] = texture;
				std::cout << "Loaded texture: " << texturePath << std::endl;
				return texture;
			} else {
				std::cerr << "Warning: Failed to load texture: " << texturePath << std::endl;
				return nullptr;
			}
		}
		return nullptr;
	}
} // namespace Engine
