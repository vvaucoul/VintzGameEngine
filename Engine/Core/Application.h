/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:18:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/28 11:26:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace Engine {

	// Forward declare Shader
	class Shader;

	// Enum for rendering modes
	enum class RenderMode {
		Default, // PBR
		Unlit,
		Wireframe
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

		// Add static members for shaders and render mode
		static Shader *s_PBRShader;
		static Shader *s_UnlitShader;
		static Shader *s_WireframeShader;
		static RenderMode s_CurrentRenderMode;
	};

} // namespace Engine