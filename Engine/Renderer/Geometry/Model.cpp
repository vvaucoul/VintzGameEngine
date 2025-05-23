/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:49:21 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 10:50:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model.h"
#include "Mesh.h"
#include "Renderer/Materials/DefaultMaterial.h"
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
		// Get the default material once outside the loop if needed
		static std::shared_ptr<MaterialPBR> defaultMaterial = GetDefaultMaterial();

		for (const auto &sub : m_SubMeshes) {
			// Use the submesh's material or the default material
			const auto &material = sub.material ? sub.material : defaultMaterial;

			// Bind material textures and set uniforms
			// --- Base PBR ---
			// Albedo (Unit 0)
			if (material->hasAlbedoMap && material->albedoMap) {
				material->albedoMap->Bind(0);
				shader.SetUniformInt("u_AlbedoMap", 0);
				shader.SetUniformInt("u_HasAlbedoMap", 1);
			} else {
				shader.SetUniformInt("u_HasAlbedoMap", 0);
				shader.SetUniformVec3("u_AlbedoColor", material->albedoColor);
			}
			// Normal (Unit 1)
			if (material->hasNormalMap && material->normalMap) {
				material->normalMap->Bind(1);
				shader.SetUniformInt("u_NormalMap", 1);
				shader.SetUniformInt("u_HasNormalMap", 1);
			} else {
				shader.SetUniformInt("u_HasNormalMap", 0);
			}
			// Metallic (Unit 2)
			if (material->hasMetallicMap && material->metallicMap) {
				material->metallicMap->Bind(2);
				shader.SetUniformInt("u_MetallicMap", 2);
				shader.SetUniformInt("u_HasMetallicMap", 1);
			} else {
				shader.SetUniformInt("u_HasMetallicMap", 0);
				shader.SetUniformFloat("u_Metallic", material->metallic);
			}
			// Roughness (Unit 3)
			if (material->hasRoughnessMap && material->roughnessMap) {
				material->roughnessMap->Bind(3);
				shader.SetUniformInt("u_RoughnessMap", 3);
				shader.SetUniformInt("u_HasRoughnessMap", 1);
			} else {
				shader.SetUniformInt("u_HasRoughnessMap", 0);
				shader.SetUniformFloat("u_Roughness", material->roughness);
			}
			// AO (Unit 4)
			if (material->hasAOMap && material->aoMap) {
				material->aoMap->Bind(4);
				shader.SetUniformInt("u_AOMap", 4);
				shader.SetUniformInt("u_HasAOMap", 1);
			} else {
				shader.SetUniformInt("u_HasAOMap", 0);
				shader.SetUniformFloat("u_AO", material->ao); // Set AO factor if no map
			}

			// --- Extended Features ---
			// Emissive (Unit 5)
			if (material->hasEmissiveMap && material->emissiveMap) {
				material->emissiveMap->Bind(5);
				shader.SetUniformInt("u_EmissiveMap", 5);
				shader.SetUniformInt("u_HasEmissiveMap", 1);
			} else {
				shader.SetUniformInt("u_HasEmissiveMap", 0);
				shader.SetUniformVec3("u_EmissiveColor", material->emissiveColor);
			}
			// Opacity (Unit 6)
			if (material->hasOpacityMap && material->opacityMap) {
				material->opacityMap->Bind(6);
				shader.SetUniformInt("u_OpacityMap", 6);
				shader.SetUniformInt("u_HasOpacityMap", 1);
			} else {
				shader.SetUniformInt("u_HasOpacityMap", 0);
				shader.SetUniformFloat("u_Opacity", material->opacity);
			}
			// Height/Parallax (Unit 7)
			if (material->hasHeightMap && material->heightMap) {
				material->heightMap->Bind(7);
				shader.SetUniformInt("u_HeightMap", 7);
				shader.SetUniformInt("u_HasHeightMap", 1);
				shader.SetUniformFloat("u_ParallaxScale", material->parallaxScale);
			} else {
				shader.SetUniformInt("u_HasHeightMap", 0);
				// Parallax scale is only relevant if there's a height map
				// shader.SetUniformFloat("u_ParallaxScale", 0.0f); // Or set based on material->parallaxScale if needed without map
			}
			// Clearcoat (Unit 8)
			if (material->hasClearcoatMap && material->clearcoatMap) {
				material->clearcoatMap->Bind(8);
				shader.SetUniformInt("u_ClearcoatMap", 8);
				shader.SetUniformInt("u_HasClearcoatMap", 1);
			} else {
				shader.SetUniformInt("u_HasClearcoatMap", 0);
			}
			// Always set clearcoat factors
			shader.SetUniformFloat("u_Clearcoat", material->clearcoat);
			shader.SetUniformFloat("u_ClearcoatRoughness", material->clearcoatRoughness);

			// Anisotropy (Unit 9)
			if (material->hasAnisotropyMap && material->anisotropyMap) {
				material->anisotropyMap->Bind(9);
				shader.SetUniformInt("u_AnisotropyMap", 9);
				shader.SetUniformInt("u_HasAnisotropyMap", 1);
			} else {
				shader.SetUniformInt("u_HasAnisotropyMap", 0);
			}
			// Always set anisotropy factors
			shader.SetUniformFloat("u_Anisotropy", material->anisotropy);
			shader.SetUniformVec3("u_AnisotropyDirection", material->anisotropyDirection);

			// Subsurface (Unit 10)
			if (material->hasSubsurfaceMap && material->subsurfaceMap) {
				material->subsurfaceMap->Bind(10);
				shader.SetUniformInt("u_SubsurfaceMap", 10);
				shader.SetUniformInt("u_HasSubsurfaceMap", 1);
			} else {
				shader.SetUniformInt("u_HasSubsurfaceMap", 0);
			}
			// Always set subsurface factors
			shader.SetUniformFloat("u_Subsurface", material->subsurface);
			shader.SetUniformVec3("u_SubsurfaceColor", material->subsurfaceColor);

			// Sheen (No specific map in shader, only factors)
			shader.SetUniformVec3("u_SheenColor", material->sheenColor);
			shader.SetUniformFloat("u_SheenRoughness", material->sheenRoughness);

			// Draw mesh geometry
			sub.mesh->Draw();

			// Optional: Unbind textures after drawing each submesh?
			// Generally not necessary if the next submesh rebinds or if state is reset elsewhere.
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
