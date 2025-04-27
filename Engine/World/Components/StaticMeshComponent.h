/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticMeshComponent.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:14:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 01:19:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "World/ActorComponent.h"
#include <glm/glm.hpp>
#include <memory> // For std::shared_ptr
#include <string>

namespace Engine {

	class Actor;
	class Mesh;
	class Model;
	class Shader;
	class MaterialPBR;

	class StaticMeshComponent : public ActorComponent {
	public:
		// Constructor for primitive meshes (takes raw pointer, assumes external ownership)
		StaticMeshComponent(Actor *owner, Mesh *mesh);
		// Constructor for loaded models (takes ownership via unique_ptr)
		StaticMeshComponent(Actor *owner, const std::string &objPath, std::shared_ptr<MaterialPBR> material = nullptr);
		~StaticMeshComponent() override;

		void Render(Shader &shader);
		void RenderDepth(Shader &depthShader);
		void SetMaterial(std::shared_ptr<MaterialPBR> material);

	private:
		Mesh *m_Mesh; // Changed to raw pointer
		std::unique_ptr<Model> m_Model;
		std::shared_ptr<MaterialPBR> m_Material;
	};

} // namespace Engine