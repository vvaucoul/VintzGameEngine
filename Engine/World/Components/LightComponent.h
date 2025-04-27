#pragma once

#include "World/ActorComponent.h"
#include <glm/glm.hpp>

// Forward declaration
namespace Engine {
	class Shader;
}

namespace Engine {

	/**
	 * @class LightComponent
	 * @brief Base class for all light types in the scene.
	 *
	 * Inherits from ActorComponent and provides common properties like color and intensity.
	 * Derived classes must implement the SetupUniforms method.
	 */
	class LightComponent : public ActorComponent {
	public:
		/**
		 * @brief Constructor.
		 * @param owner The Actor that owns this component.
		 * @param color The color of the light.
		 * @param intensity The brightness of the light.
		 */
		LightComponent(Actor *owner, const glm::vec3 &color = glm::vec3(1.0f), float intensity = 1.0f);

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~LightComponent() = default;

		// --- Getters ---
		const glm::vec3 &GetColor() const { return m_Color; }
		float GetIntensity() const { return m_Intensity; }

		// --- Setters ---
		void SetColor(const glm::vec3 &color) { m_Color = color; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }

		/**
		 * @brief Pure virtual function to set up light-specific uniforms in a shader.
		 * @param shader The shader to set uniforms for.
		 * @param index The index of the light (for arrays like point/spot lights).
		 */
		virtual void SetupUniforms(Shader &shader, int index) const = 0;

	protected:
		glm::vec3 m_Color;
		float m_Intensity;
	};

} // namespace Engine
