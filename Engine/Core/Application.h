/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:18:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 21:28:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderer/Framebuffer.h"
#include <memory>

namespace Engine {

	// Forward declare Shader
	class Shader;

	// Enum for rendering modes
	enum class RenderMode {
		Default, // PBR
		Unlit,
		Wireframe
	};

	// Enum for rendering paths
	enum class RenderingPath {
		Forward,
		Deferred
	};

	class Application {
	public:
		Application();
		~Application();

		// Starts the engine
		void Run();

	private:
		void Init();
		void MainLoop();
		void Shutdown();

		static Shader *s_PBRShader;
		static Shader *s_UnlitShader;
		static Shader *s_WireframeShader;

		static std::unique_ptr<Shader> s_GBufferShader;			 // Shader for G-Buffer pass
		static std::unique_ptr<Shader> s_DeferredLightingShader; // Shader for Deferred lighting pass
		static RenderMode s_CurrentRenderMode;
		static RenderingPath s_CurrentRenderingPath;	  // Current rendering path
		static std::unique_ptr<Framebuffer> s_GBufferFBO; // G-Buffer Framebuffer
	};

} // namespace Engine