#include "Model.h"
#include "Mesh.h"
#include "Renderer/MaterialPBR.h" // Ensure included
#include "Renderer/Shader.h"
#include "Renderer/Texture.h" // Ensure included

#include <assimp/Importer.hpp>	// Include Assimp
#include <assimp/postprocess.h> // Include Assimp
#include <assimp/scene.h>		// Include Assimp
#include <fstream>				// Include for std::ifstream
#include <iostream>
#include <map> // Include for std::map (used via Model.h)

namespace Engine {

	Model::Model(const std::string &path) {
		LoadModel(path);
	}

	Model::~Model() = default;

	void Model::Draw(Shader &shader) const {
		for (const auto &sub : m_SubMeshes) {
			// Apply material if available
			if (sub.material) { // Use sub.material
				// Bind textures and set uniforms based on sub.material
				// Example: Bind Albedo
				if (sub.material->hasAlbedoMap && sub.material->albedoMap) { // Use sub.material
					sub.material->albedoMap->Bind(0);						 // Use sub.material
					shader.SetUniformInt("albedoMap", 0);
					shader.SetUniformInt("hasAlbedoMap", sub.material->hasAlbedoMap ? 1 : 0);
				} else {
					shader.SetUniformInt("hasAlbedoMap", 0);
					shader.SetUniformVec3("albedoColor", sub.material->albedoColor); // Use sub.material
				}
				// Bind Normal Map
				if (sub.material->hasNormalMap && sub.material->normalMap) { // Use sub.material
					sub.material->normalMap->Bind(1);						 // Use sub.material
					shader.SetUniformInt("normalMap", 1);
					shader.SetUniformInt("hasNormalMap", sub.material->hasNormalMap ? 1 : 0);
				} else {
					shader.SetUniformInt("hasNormalMap", 0);
				}
				// Bind ORM/AO Map and set uniforms
				if (sub.material->hasAOMap && sub.material->aoMap) { // Use sub.material
					sub.material->aoMap->Bind(2);					 // Use sub.material // Bind separate AO to unit 2
					shader.SetUniformInt("aoMap", 2);
					shader.SetUniformInt("hasAoMap", 1);
				} else {
					shader.SetUniformInt("hasAoMap", 0);
				}
				shader.SetUniformFloat("metallic", sub.material->metallic);
				shader.SetUniformFloat("roughness", sub.material->roughness);
				shader.SetUniformFloat("ao", sub.material->ao);
			} else {
				// Set default material properties if no material is assigned to the submesh
				shader.SetUniformInt("hasAlbedoMap", 0);
				shader.SetUniformVec3("albedoColor", {0.8f, 0.8f, 0.8f}); // Default grey
				shader.SetUniformInt("hasNormalMap", 0);
				shader.SetUniformInt("hasAoMap", 0);
				shader.SetUniformFloat("metallic", 0.1f);
				shader.SetUniformFloat("roughness", 0.8f);
				shader.SetUniformFloat("ao", 1.0f);
			}

			// Draw the mesh geometry (shader is already bound)
			sub.mesh->Draw(); // Call Draw without shader argument
		}
	}

	void Model::DrawGeometry([[maybe_unused]] Shader &shader) const {
		// This method is intended to draw only geometry, assuming material uniforms
		// are set externally (e.g., by StaticMeshComponent).
		for (const auto &sub : m_SubMeshes) {
			sub.mesh->Draw(); // Call Draw without shader argument
		}
	}

	void Model::LoadModel(const std::string &path) {
		Assimp::Importer importer; // Should work now
		const aiScene *scene = importer.ReadFile(path,
												 aiProcess_Triangulate |
													 aiProcess_FlipUVs |
													 aiProcess_CalcTangentSpace |
													 aiProcess_GenSmoothNormals);		// Flags should work now
		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) { // Check flags too
			std::cerr << "Assimp load error: " << importer.GetErrorString() << "\n";
			return;
		}
		// Store the directory path of the model file
		m_Directory = path.substr(0, path.find_last_of('/'));
		if (m_Directory == path) { // Handle case where path has no '/'
			m_Directory = ".";
		}

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode *node, const aiScene *scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			m_SubMeshes.push_back(ProcessMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			ProcessNode(node->mChildren[i], scene);
	}

	Model::SubMesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<Vertex> verts;
		std::vector<unsigned int> indices;
		std::shared_ptr<MaterialPBR> material = nullptr; // Initialize material

		verts.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex v;
			// Position
			v.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
			// Normal
			v.Normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f, 1.0f, 0.0f); // Default up if no normal
			// TexCoords
			v.TexCoords = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
			// Tangent
			v.Tangent = mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z) : glm::vec3(1.0f, 0.0f, 0.0f); // Default +X if no tangent
			// Bitangent
			v.Bitangent = mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z) : glm::vec3(0.0f, 0.0f, 1.0f); // Default +Z if no bitangent

			verts.push_back(v);
		}

		indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			// Make sure we only process triangles if aiProcess_Triangulate was successful
			if (face.mNumIndices == 3) {
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}
		}

		// Process material if the mesh has one assigned and the index is valid
		if (mesh->mMaterialIndex < scene->mNumMaterials) { // Check index validity
			aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];
			material		  = std::make_shared<MaterialPBR>(); // Create a new PBR material

			// Load textures (simplified example)
			material->albedoMap = LoadMaterialTexture(aiMat, aiTextureType_DIFFUSE);
			material->normalMap = LoadMaterialTexture(aiMat, aiTextureType_NORMALS); // Or aiTextureType_HEIGHT for bump maps sometimes

			// Try loading individual maps if ORM isn't found
			material->aoMap	  = LoadMaterialTexture(aiMat, aiTextureType_AMBIENT_OCCLUSION); // Or aiTextureType_LIGHTMAP
			auto roughnessMap = LoadMaterialTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS);
			auto metallicMap  = LoadMaterialTexture(aiMat, aiTextureType_METALNESS);
			// If individual maps exist, you'd need shader logic to handle them.
			// For simplicity, we'll rely on ORM or fallback to values.
			// If roughness/metallic maps were loaded, set flags/handle them
			// material->hasRoughnessMap = (roughnessMap != nullptr);
			// material->hasMetallicMap = (metallicMap != nullptr);

			// Set flags based on loaded textures
			material->hasAlbedoMap = (material->albedoMap != nullptr);
			material->hasNormalMap = (material->normalMap != nullptr);
			material->hasAOMap	   = (material->aoMap != nullptr); // Only relevant if ORM isn't loaded

			// Get material properties (colors, factors) - Add error checking
			aiColor4D color;
			if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
				material->albedoColor = {color.r, color.g, color.b};
			}
			// Get metallic/roughness factors (may need specific keys like $mat.metallicFactor, $mat.roughnessFactor)
			float factor = 0.0f;
			// Use PBR specific keys if available (introduced in later Assimp versions)
			if (aiGetMaterialFloat(aiMat, AI_MATKEY_METALLIC_FACTOR, &factor) == AI_SUCCESS) {
				material->metallic = factor;
			} else if (aiGetMaterialFloat(aiMat, "$mat.metallicFactor", 0, 0, &factor) == AI_SUCCESS) { // Try glTF 2.0 key
				material->metallic = factor;
			} else {
				// Fallback or default if not found
				material->metallic = 0.1f; // Default slightly metallic
			}

			if (aiGetMaterialFloat(aiMat, AI_MATKEY_ROUGHNESS_FACTOR, &factor) == AI_SUCCESS) {
				material->roughness = factor;
			} else if (aiGetMaterialFloat(aiMat, "$mat.roughnessFactor", 0, 0, &factor) == AI_SUCCESS) { // Try glTF 2.0 key
				material->roughness = factor;
			} else {
				// Fallback or default
				material->roughness = 0.8f; // Default quite rough
			}
			// AO factor might be part of ORM texture or a separate value
			material->ao = 1.0f; // Default full AO
		}

		// Create the Mesh object (now owned by SubMesh)
		auto meshPtr = std::make_unique<Mesh>(verts, indices);

		// Return SubMesh containing the Mesh and Material (should work now)
		return {std::move(meshPtr), material};
	}

	std::shared_ptr<Texture> Model::LoadMaterialTexture(aiMaterial *mat, aiTextureType type) {
		if (mat->GetTextureCount(type) > 0) {
			aiString str;
			mat->GetTexture(type, 0, &str); // Get the first texture of this type
			std::string textureFile = str.C_Str();

			// Prevent loading invalid texture names (e.g., "*0")
			if (textureFile.empty() || textureFile[0] == '*') {
				return nullptr;
			}

			std::string texturePath = m_Directory + "/" + textureFile;

			// Check cache
			if (m_LoadedTextures.count(texturePath)) { // Should work now
				return m_LoadedTextures[texturePath];
			}

			// Check if file exists before loading
			std::ifstream f(texturePath.c_str()); // Should work now
			if (!f.good()) {
				std::cerr << "Warning: Texture file not found: " << texturePath << std::endl;
				// Try loading from the base directory if not found in relative path
				texturePath = textureFile;				   // Use only filename
				std::ifstream f_base(texturePath.c_str()); // Should work now
				if (!f_base.good()) {
					std::cerr << "Warning: Texture file not found in base directory either: " << texturePath << std::endl;
					return nullptr;
				}
			}
			f.close(); // Close the ifstream if it was opened

			// Load the texture
			auto texture = std::make_shared<Texture>(texturePath);
			if (texture->GetID() != 0) {									 // Call GetID() - should work now
				m_LoadedTextures[texturePath] = texture;					 // Cache it - Should work now
				std::cout << "Loaded texture: " << texturePath << std::endl; // Debug output
				return texture;
			} else {
				std::cerr << "Warning: Failed to load texture: " << texturePath << std::endl;
				return nullptr;
			}
		}
		return nullptr;
	}

}
