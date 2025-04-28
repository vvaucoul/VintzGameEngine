#pragma once

#include "Renderer/Shader.h"				 // For SetupUniforms signature
#include "World/Components/SceneComponent.h" // Include SceneComponent
#include <glm/glm.hpp>

namespace Engine {

	class Actor;

	/**
	 * @class LightComponent
	 * @brief Base class for all light types. Inherits SceneComponent for transform.
	 */
	class LightComponent : public SceneComponent { // Inherit from SceneComponent
	public:
		/**
		 * @brief Constructor.
		 * @param owner The Actor that owns this component.
		 * @param color The color of the light.
		 * @param intensity The brightness of the light.
		 */
		LightComponent(Actor *owner, const glm::vec3 &color = {1.0f, 1.0f, 1.0f}, float intensity = 1.0f);

		~LightComponent() override = default;

		/**
		 * @brief Pure virtual function to set up light-specific uniforms in a shader.
		 * @param shader The shader to set uniforms for.
		 * @param index The index of the light (for arrays like point/spot lights).
		 */
		virtual void SetupUniforms(Shader &shader, int index) const = 0;

		// --- Getters ---
		const glm::vec3 &GetColor() const { return m_Color; }
		float GetIntensity() const { return m_Intensity; }

		// --- Setters ---
		void SetColor(const glm::vec3 &color) { m_Color = color; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }

	protected:
		glm::vec3 m_Color;
		float m_Intensity;

		// Position and Rotation are now handled by the base SceneComponent
	};

} // namespace Engine
