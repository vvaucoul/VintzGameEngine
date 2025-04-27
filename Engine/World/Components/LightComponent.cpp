#include "LightComponent.h"
// No need to include Shader.h here, it's only needed for the virtual function declaration

namespace Engine {

	LightComponent::LightComponent(Actor *owner, const glm::vec3 &color, float intensity)
		: ActorComponent(owner), m_Color(color), m_Intensity(intensity) {
		// Base class constructor
	}

	// Getters and Setters are inline in the header

} // namespace Engine
