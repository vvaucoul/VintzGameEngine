/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticMeshComponent.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:14:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 23:12:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Core/Application.h"					// Include Application for RenderMode enum
#include "Renderer/Materials/DefaultMaterial.h" // Include default material getter
#include "World/ActorComponent.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Engine {

	class Actor;
	class Mesh;
	class Model;
	class Shader;
	class MaterialPBR;

	/**
	 * @brief Component for rendering static meshes or models.
	 *
	 * - Can render a primitive Mesh (externally owned) or a loaded Model (owned).
	 * - Always has a PBR material (defaults if none provided).
	 */
	class StaticMeshComponent : public ActorComponent {
	public:
		/**
		 * @brief Construct with a primitive Mesh (ownership is external).
		 * @param owner Owning Actor.
		 * @param mesh Pointer to Mesh (not owned).
		 */
		StaticMeshComponent(Actor *owner, Mesh *mesh);

		/**
		 * @brief Construct with a loaded Model from file (ownership is internal).
		 * Uses the default material if `material` is nullptr.
		 * @param owner Owning Actor.
		 * @param objPath Path to model file (e.g. .obj).
		 * @param material Optional PBR material. If nullptr, the default material is used.
		 */
		StaticMeshComponent(Actor *owner, const std::string &objPath, std::shared_ptr<MaterialPBR> material = nullptr);

		/// Destructor.
		~StaticMeshComponent() override;

		/**
		 * @brief Render the mesh/model with the given shader and mode.
		 * @param shader Shader to use for rendering.
		 * @param mode The current rendering mode.
		 */
		void Render(Shader &shader, RenderMode mode);

		/**
		 * @brief Render only depth (for shadow mapping, etc).
		 * @param depthShader Shader for depth rendering.
		 */
		void RenderDepth(Shader &depthShader);

		/**
		 * @brief Renders the geometry of the mesh/model without material setup.
		 * Used primarily for the G-Buffer pass in deferred shading.
		 * @param shader The shader to use (typically the G-Buffer shader).
		 */
		void RenderGeometry(Shader &shader);

		/**
		 * @brief Set the PBR material.
		 * @param material Shared pointer to MaterialPBR.
		 */
		void SetMaterial(std::shared_ptr<MaterialPBR> material);

	private:
		Mesh *m_Mesh = nullptr;					 ///< Non-owning pointer to primitive mesh.
		std::unique_ptr<Model> m_Model;			 ///< Owning pointer to loaded model.
		std::shared_ptr<MaterialPBR> m_Material; ///< Shared PBR material.
	};

} // namespace Engine