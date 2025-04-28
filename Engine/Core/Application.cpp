/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:19:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/29 01:06:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Application.cpp — Main application entry point and loop for VintzGameEngine.
// Handles window creation, OpenGL context, camera, world, plugins, and main loop.

#include "Core/Application.h"
#include "Renderer/Camera.h"
#include "Renderer/GPUResources/UniformBuffer.h"
#include "Renderer/Geometry/Model.h"
#include "Renderer/Materials/DefaultMaterial.h" // Include DefaultMaterial header
#include "Renderer/Materials/MaterialPBR.h"
#include "Renderer/Pipeline/PostProcessor.h"
#include "Renderer/Pipeline/ShadowMap.h"
#include "Renderer/Primitives/Primitives.h"
#include "Renderer/Shaders/Shader.h"
#include "World/Actor.h"
#include "World/Components/DirectionalLightComponent.h"
#include "World/Components/LightComponent.h"
#include "World/Components/PointLightComponent.h"
#include "World/Components/SceneComponent.h"
#include "World/Components/SpotLightComponent.h"
#include "World/Components/StaticMeshComponent.h"
#include "World/World.h"

// Core Plugins (dynamic module loading for plugins)
#include "CorePlugins/DynamicModule.h"

// OpenGL and windowing includes
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// STL includes
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Renderer/Geometry/Mesh.h" // Include Mesh for s_PrimitiveMeshes type

namespace Engine {

	// Define static members
	GLFWwindow *s_Window = nullptr;
	Camera *s_Camera	 = nullptr;
	// Shader *s_Shader								  = nullptr; // Replaced by specific shaders
	UniformBuffer *s_UBO						   = nullptr;
	World *s_World								   = nullptr;
	std::unique_ptr<PostProcessor> s_PostProcessor = nullptr;
	std::unique_ptr<Engine::ShadowMap> s_ShadowMap = nullptr;
	std::unique_ptr<Engine::Shader> s_DepthShader  = nullptr;
	std::vector<std::unique_ptr<Mesh>> s_PrimitiveMeshes;
	std::vector<std::unique_ptr<DynamicModule>> s_Plugins;
	bool s_FirstMouse	  = true;
	float s_LastX		  = 0.0f;
	float s_LastY		  = 0.0f;
	bool s_IsCameraActive = false;

	RenderingPath Application::s_CurrentRenderingPath			  = RenderingPath::Forward; // Default to Forward
	std::unique_ptr<Framebuffer> Application::s_GBufferFBO		  = nullptr;
	std::unique_ptr<Shader> Application::s_GBufferShader		  = nullptr;
	std::unique_ptr<Shader> Application::s_DeferredLightingShader = nullptr;

	// Define new static members
	Shader *Application::s_PBRShader			= nullptr;
	Shader *Application::s_UnlitShader			= nullptr;
	Shader *Application::s_WireframeShader		= nullptr;
	RenderMode Application::s_CurrentRenderMode = RenderMode::Default;

	// Helper to render a full screen quad (can be moved to PostProcessor or Renderer class)
	static unsigned int quadVAO = 0;
	static unsigned int quadVBO;
	void renderQuad() {
		if (quadVAO == 0) {
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,
				1.0f,
				0.0f,
				0.0f,
				1.0f,
				-1.0f,
				-1.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f,
				1.0f,
				0.0f,
				1.0f,
				1.0f,
				1.0f,
				-1.0f,
				0.0f,
				1.0f,
				0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	static void FramebufferSizeCallback([[maybe_unused]] GLFWwindow *window, int width, int height) {
		glViewport(0, 0, width, height);
		if (s_Camera)
			s_Camera->SetAspectRatio(float(width) / float(height));
		// Optionally resize PostProcessor FBOs here if needed
		// if (s_PostProcessor) s_PostProcessor->Resize(width, height);
	}

	static void MouseCallback(GLFWwindow * /*window*/, double xpos, double ypos) {
		// Only process mouse look when camera is active (RMB held)
		if (!s_IsCameraActive) {
			// s_FirstMouse = true; // No longer needed here, handled in MouseButtonCallback
			return;
		}

		if (s_FirstMouse) {
			s_LastX		 = float(xpos);
			s_LastY		 = float(ypos);
			s_FirstMouse = false;
			return;
		}
		float dx = float(xpos) - s_LastX;
		float dy = s_LastY - float(ypos); // reversed since y-coordinates go from bottom to top
		s_LastX	 = float(xpos);
		s_LastY	 = float(ypos);

		if (s_Camera) {
			s_Camera->ProcessMouseMovement(dx, dy);
		}
	}

	// New callback for mouse button events
	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int /*mods*/) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (action == GLFW_PRESS) {
				s_IsCameraActive = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide and capture cursor
				s_FirstMouse = true;										 // Reset first mouse on click to avoid jump
			} else if (action == GLFW_RELEASE) {
				s_IsCameraActive = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Release cursor
			}
		}
	}

	Application::Application() = default;

	Application::~Application() = default;

	void Application::Run() {
		Init();
		MainLoop();
		Shutdown();
	}

	void Application::Init() {
		// Initialize the core plugins
		// DynamicModule plugin("plugins/MyPlugin/libMyPlugin.so");
		// plugin.load();
		// plugin.Init(engineContext);

		if (!glfwInit()) exit(EXIT_FAILURE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4); // Request 4x MSAA

		// Define window dimensions (consider making these constants or members)
		const unsigned int windowWidth	= 1280;
		const unsigned int windowHeight = 720;

		// Get primary monitor and its video mode
		GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode		= glfwGetVideoMode(primaryMonitor);

		if (!mode) {
			std::cerr << "[ERROR] Failed to get primary monitor video mode." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		// Calculate window position for centering
		int windowPosX = (mode->width - windowWidth) / 2;
		int windowPosY = (mode->height - windowHeight) / 2;

		// Create the window (initially potentially not centered)
		s_Window = glfwCreateWindow(windowWidth, windowHeight, "Vintz Game Engine", nullptr, nullptr);
		if (!s_Window) {
			glfwTerminate(); // Terminate GLFW before exiting
			exit(EXIT_FAILURE);
		}

		// Set the calculated window position to center it
		glfwSetWindowPos(s_Window, windowPosX, windowPosY);

		glfwMakeContextCurrent(s_Window);
		glfwSetFramebufferSizeCallback(s_Window, FramebufferSizeCallback);
		glfwSetCursorPosCallback(s_Window, MouseCallback);
		glfwSetMouseButtonCallback(s_Window, MouseButtonCallback);
		glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Start with cursor hidden and captured

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			glfwTerminate(); // Terminate GLFW before exiting
			exit(EXIT_FAILURE);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);						   // Enable MSAA
		glEnable(GL_BLEND);								   // Enable blending globally (needed for billboards/transparency)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

		// Camera
		s_Camera = new Camera({0.0f, 2.0f, 8.0f}, 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);

		// --- Load Shaders ---
		s_PBRShader		  = new Shader("Shaders/Core/pbr.vert", "Shaders/Core/pbr.frag");
		s_UnlitShader	  = new Shader("Shaders/Core/unlit.vert", "Shaders/Core/unlit.frag");
		s_WireframeShader = new Shader("Shaders/Core/wireframe.vert", "Shaders/Core/wireframe.frag");
		s_DepthShader	  = std::make_unique<Engine::Shader>("Shaders/Core/depth.vert", "Shaders/Core/depth.frag");

		// Load Deferred Shaders
		s_GBufferShader			 = std::make_unique<Engine::Shader>("Shaders/Core/Deferred/gbuffer.vert", "Shaders/Core/Deferred/gbuffer.frag");
		s_DeferredLightingShader = std::make_unique<Engine::Shader>("Shaders/Core/Deferred/deferred_lighting.vert", "Shaders/Core/Deferred/deferred_lighting.frag");

		if (!s_PBRShader->IsValid() || !s_UnlitShader->IsValid() || !s_WireframeShader->IsValid()) {
			std::cerr << "[ERROR] Failed to load forward shaders." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		if (!s_GBufferShader->IsValid() || !s_DeferredLightingShader->IsValid()) {
			std::cerr << "[ERROR] Failed to load deferred shaders." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		s_GBufferFBO = std::make_unique<Framebuffer>(windowWidth, windowHeight);
		// Attachment 0: Position (World Space) + Depth? (RGBA16F or RGBA32F)
		s_GBufferFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
		// Attachment 1: Normal (World Space) + Metallic? (RGBA16F)
		s_GBufferFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
		// Attachment 2: Albedo + Roughness? (RGBA8)
		s_GBufferFBO->AddColorTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
		// Attachment 3: AO + Emissive + Specular? (RGBA8 or RGBA16F) - Adjust as needed
		s_GBufferFBO->AddColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT);
		// Depth/Stencil Attachment
		s_GBufferFBO->AddDepthStencil();
		s_GBufferFBO->Build(); // Check for completeness

		s_UBO = new UniformBuffer(sizeof(glm::mat4) * 2, 0);

		// Initialize PostProcessor
		s_PostProcessor = std::make_unique<Engine::PostProcessor>(windowWidth, windowHeight);

		// Initialize ShadowMap
		s_ShadowMap = std::make_unique<Engine::ShadowMap>(2048, 2048);

		// Initialize Depth Shader
		// Update paths to Core directory
		s_DepthShader = std::make_unique<Engine::Shader>(
			"Shaders/Core/depth.vert",
			"Shaders/Core/depth.frag");

		if (!s_DepthShader->IsValid()) {
			std::cerr << "[ERROR] Failed to load depth shader." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		// World & Actors
		s_World = new World();

		// Create and store primitive meshes first
		s_PrimitiveMeshes.push_back(Primitives::CreateSphere());   // Index 0
		s_PrimitiveMeshes.push_back(Primitives::CreatePlane());	   // Index 1
		s_PrimitiveMeshes.push_back(Primitives::CreateCylinder()); // Index 2
		s_PrimitiveMeshes.push_back(Primitives::CreateCone());	   // Index 3
		s_PrimitiveMeshes.push_back(Primitives::CreateTorus());	   // Index 4
		s_PrimitiveMeshes.push_back(Primitives::CreateCube());	   // Index 5 (Added primitive cube)

		// --- PBR Materials ---
		// Dirt Material for the ground plane - Load textures at native resolution
		auto worldMaterial = std::make_shared<Engine::MaterialPBR>();
		worldMaterial->SetAlbedoMap("assets/textures/World_Diffuse.png");
		worldMaterial->SetNormalMap("assets/textures/World_Normal.png");
		worldMaterial->metallic	 = 0.2f;
		worldMaterial->roughness = 0.75f;
		worldMaterial->ao		 = 1.0f;

		// Simple Metallic Material for the sphere - No textures
		auto metalMat		  = std::make_shared<Engine::MaterialPBR>();
		metalMat->albedoColor = {0.8f, 0.8f, 0.85f};
		metalMat->metallic	  = 0.9f;
		metalMat->roughness	  = 0.2f;
		metalMat->ao		  = 1.0f;
		metalMat->specular	  = 0.5f;

		// Simple Plastic Material for cone/cylinder - Load albedo resized to 256x256 using Lanczos
		auto plasticMat = std::make_shared<Engine::MaterialPBR>();
		// Example: Load a dummy texture and resize it
		plasticMat->albedoColor = {0.8f, 0.1f, 0.1f}; // Red albedo
		plasticMat->metallic	= 0.05f;
		plasticMat->roughness	= 0.4f;
		plasticMat->ao			= 1.0f;

		auto bricksMat = std::make_shared<Engine::MaterialPBR>();
		bricksMat->SetAlbedoMap("assets/textures/bricks/Bricks_Diffuse.png");
		bricksMat->SetNormalMap("assets/textures/bricks/Bricks_Normal.png");
		bricksMat->roughness = 0.5f; // Medium roughness
		bricksMat->metallic	 = 0.0f; // Non-metallic
		bricksMat->SetAOMap("assets/textures/bricks/Bricks_AmbientOcclusion.png");
		bricksMat->SetHeightMap("assets/textures/bricks/Bricks_Height.png");
		bricksMat->SetSpecularMap("assets/textures/bricks/Bricks_Specular.png");

		// --- Actor Setup ---
		// Ground Plane (using Primitive) - Apply Dirt Material
		auto &planeActor = s_World->SpawnActor();
		planeActor.GetRootComponent()->SetPosition({0.0f, 0.0f, 0.0f});
		planeActor.GetRootComponent()->SetScale({10.0f, 1.0f, 10.0f});
		auto &planeMeshComp = planeActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[1].get());
		planeMeshComp.SetMaterial(worldMaterial);

		// Cube (using Model) - Use Default Material
		auto &objActor = s_World->SpawnActor();
		objActor.GetRootComponent()->SetPosition({-3.0f, 0.5f, -2.0f});
		objActor.AddComponent<StaticMeshComponent>("assets/models/cube.obj").SetMaterial(GetDefaultMaterial()); // Use default

		// Sphere (using Primitive) - Apply Metallic Material
		auto &sphereActor = s_World->SpawnActor();
		sphereActor.GetRootComponent()->SetPosition({0.0f, 0.75f, 0.0f});
		sphereActor.GetRootComponent()->SetScale({0.75f, 0.75f, 0.75f});
		auto &sphereMeshComp = sphereActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[0].get());
		sphereMeshComp.SetMaterial(metalMat);

		// Cylinder (using Primitive) - Apply Plastic Material
		auto &cylinderActor = s_World->SpawnActor();
		cylinderActor.GetRootComponent()->SetPosition({3.0f, 0.5f, -1.0f});
		auto &cylinderMeshComp = cylinderActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[2].get());
		cylinderMeshComp.SetMaterial(plasticMat);

		// Cone (using Primitive) - Apply Plastic Material
		auto &coneActor = s_World->SpawnActor();
		coneActor.GetRootComponent()->SetPosition({-1.5f, 0.5f, 2.5f});
		auto &coneMeshComp = coneActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[3].get());
		coneMeshComp.SetMaterial(plasticMat);

		// Torus (using Primitive) - Use Default Material
		auto &torusActor = s_World->SpawnActor();
		torusActor.GetRootComponent()->SetPosition({2.0f, 0.5f, 2.0f});
		torusActor.GetRootComponent()->SetRotation({0.0f, 45.0f, 0.0f});
		torusActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[4].get()).SetMaterial(GetDefaultMaterial()); // Use default

		// Crate 1 (using Model) - Use Default Material (assuming crate.obj doesn't define its own)
		auto &crateActor1 = s_World->SpawnActor();
		crateActor1.GetRootComponent()->SetPosition({-2.5f, 0.5f, 0.5f});
		crateActor1.GetRootComponent()->SetRotation({0.0f, -30.0f, 0.0f});
		crateActor1.AddComponent<StaticMeshComponent>("assets/models/crate.obj").SetMaterial(GetDefaultMaterial()); // Use default

		// Crate 2 (using Model) - Use Default Material
		auto &crateActor2 = s_World->SpawnActor();
		crateActor2.GetRootComponent()->SetPosition({3.5f, 0.5f, 1.5f});
		crateActor2.GetRootComponent()->SetRotation({0.0f, 60.0f, 0.0f});
		crateActor2.GetRootComponent()->SetScale({1.2f, 1.2f, 1.2f});
		crateActor2.AddComponent<StaticMeshComponent>("assets/models/crate.obj").SetMaterial(GetDefaultMaterial()); // Use default

		// Primitive Cube (using Primitive) - Use Default Material
		auto &primCubeActor = s_World->SpawnActor();
		primCubeActor.GetRootComponent()->SetPosition({0.5f, 0.5f, -3.0f});
		auto &primCubeMeshComp = primCubeActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[5].get());
		primCubeActor.AddComponent<BillboardComponent>(); // Add billboard component (no argument)
		primCubeMeshComp.SetMaterial(bricksMat);		  // Use bricks material

		// --- Lights ---
		// Directional Light (Sun)
		auto &sun = s_World->SpawnActor();
		sun.GetRootComponent()->SetRotation({-60.0f, -30.0f, 0.0f});
		// Use the DirectionalLightComponent constructor directly
		sun.AddComponent<DirectionalLightComponent>(glm::vec3(1.0f, 0.95f, 0.85f), 1.0f);
		sun.GetComponentsByClass<DirectionalLightComponent>()[0]->SetIntensity(2.0f); // Set intensity

		// Point Light (Bulb)
		auto &bulb = s_World->SpawnActor();
		bulb.GetRootComponent()->SetPosition({-1.0f, 2.0f, -1.0f});
		// Use the PointLightComponent constructor directly
		bulb.AddComponent<PointLightComponent>(glm::vec3(0.3f, 0.8f, 1.0f), 2.0f); // Default attenuation used
		bulb.GetComponentsByClass<PointLightComponent>()[0]->SetIntensity(8.0f);   // Set intensity

		auto &spotLight = s_World->SpawnActor();
		spotLight.GetRootComponent()->SetPosition({3.0f, 2.0f, 1.0f});
		spotLight.GetRootComponent()->SetRotation({-30.0f, -60.0f, 0.0f});
		// Use the SpotLightComponent constructor directly
		spotLight.AddComponent<SpotLightComponent>(glm::vec3(1.0f, 0.8f, 0.2f), 2.5f, 12.5f, 17.5f); // Default attenuation used
		spotLight.GetComponentsByClass<SpotLightComponent>()[0]->SetIntensity(20.0f);				 // Set intensity

		// Spot Light (Torch)
		auto &torch = s_World->SpawnActor();
		torch.GetRootComponent()->SetPosition({2.0f, 1.5f, 3.0f});
		torch.GetRootComponent()->SetRotation({-30.0f, -60.0f, 0.0f});
		// Use the SpotLightComponent constructor directly
		torch.AddComponent<SpotLightComponent>(glm::vec3(1.0f, 0.8f, 0.2f), 2.5f, 12.5f, 17.5f); // Default attenuation used

		// --- Initialize Plugins ---
		std::cout << "[INFO] Loading TestPlugin..." << std::endl;
		auto testMod = std::make_unique<DynamicModule>("./bin/plugins/libHelloPlugin.so");
		if (testMod->Load()) {
			std::cout << "[INFO] Loaded TestPlugin: " << testMod->GetPath() << std::endl;
			if (testMod->InitFunction)
				testMod->InitFunction(nullptr);
			s_Plugins.push_back(std::move(testMod));
		} else {
			std::cerr << "[ERROR] Failed to load plugins/libHelloPlugin.so" << std::endl;
		}
	}

	void Application::MainLoop() {
		float lastFrame = 0.0f;
		while (!glfwWindowShouldClose(s_Window)) {
			float current = float(glfwGetTime());
			float delta	  = current - lastFrame;
			lastFrame	  = current;

			// --- Input Processing ---
			// Keyboard movement
			bool forward  = glfwGetKey(s_Window, GLFW_KEY_W) == GLFW_PRESS;
			bool backward = glfwGetKey(s_Window, GLFW_KEY_S) == GLFW_PRESS;
			bool left	  = glfwGetKey(s_Window, GLFW_KEY_A) == GLFW_PRESS;
			bool right	  = glfwGetKey(s_Window, GLFW_KEY_D) == GLFW_PRESS;
			bool up		  = glfwGetKey(s_Window, GLFW_KEY_E) == GLFW_PRESS;
			bool down	  = glfwGetKey(s_Window, GLFW_KEY_Q) == GLFW_PRESS;
			if (s_Camera) {
				s_Camera->ProcessKeyboard(delta, forward, backward, left, right, up, down);
			}
			// Note: Mouse input (rotation) is handled in MouseCallback

			// Render Mode Switching
			if (glfwGetKey(s_Window, GLFW_KEY_F1) == GLFW_PRESS) {
				s_CurrentRenderMode = RenderMode::Wireframe;
			} else if (glfwGetKey(s_Window, GLFW_KEY_F2) == GLFW_PRESS) {
				s_CurrentRenderMode = RenderMode::Unlit;
			} else if (glfwGetKey(s_Window, GLFW_KEY_F3) == GLFW_PRESS) {
				s_CurrentRenderMode = RenderMode::Default;
			} else if (glfwGetKey(s_Window, GLFW_KEY_F9) == GLFW_PRESS) {
				s_CurrentRenderingPath = RenderingPath::Forward;
				std::cout << "Switched to Forward Shading" << std::endl;
			} else if (glfwGetKey(s_Window, GLFW_KEY_F10) == GLFW_PRESS) {
				s_CurrentRenderingPath = RenderingPath::Deferred;
				std::cout << "Switched to Deferred Shading" << std::endl;
			} else if (glfwGetKey(s_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(s_Window, true);
			}

			// --- Camera & UBO Update ---
			glm::mat4 proj = s_Camera->GetProjectionMatrix();
			glm::mat4 view = s_Camera->GetViewMatrix();
			s_UBO->SetData(0, sizeof(glm::mat4), &proj[0][0]);
			s_UBO->SetData(sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);

			// --- Shadow Mapping Pass ---
			// (Shadow mapping always uses the depth shader, unaffected by render mode)
			// 1) Compute light space matrix based on the directional light
			glm::vec3 lightDir = glm::vec3(-0.2f, -1.0f, -0.3f); // Default direction
			auto sunActor	   = std::find_if(
				 s_World->GetActors().begin(),
				 s_World->GetActors().end(),
				 [](const auto &a) { return a->template GetComponent<DirectionalLightComponent>() != nullptr; });
			if (sunActor != s_World->GetActors().end()) {
				auto dirLightComp = (*sunActor)->template GetComponent<DirectionalLightComponent>();
				if (dirLightComp) {
					lightDir = dirLightComp->GetDirection(); // Use the GetDirection method
				}
			}
			s_ShadowMap->ComputeLightSpaceMatrix(lightDir); // Pass the retrieved or default direction

			// 2) Render scene to depth map
			s_ShadowMap->BindForWriting();
			glViewport(0, 0, 2048, 2048); // Set viewport to shadow map size
			glClear(GL_DEPTH_BUFFER_BIT);
			s_DepthShader->Bind();
			s_DepthShader->SetUniformMat4("lightSpaceMatrix", s_ShadowMap->GetLightSpaceMatrix());
			s_World->RenderDepth(*s_DepthShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind shadow FBO

			// Reset viewport to window size
			int display_w, display_h;
			glfwGetFramebufferSize(s_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);

			// --- Main Rendering Pass ---
			// Reset viewport and clear
			glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (s_CurrentRenderingPath == RenderingPath::Forward) {
				// --- Forward Shading Path ---
				glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default

				Shader *currentShader = s_PBRShader;
				if (s_CurrentRenderMode == RenderMode::Unlit) {
					currentShader = s_UnlitShader;
				} else if (s_CurrentRenderMode == RenderMode::Wireframe) {
					currentShader = s_WireframeShader;
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				currentShader->Bind();

				// Set common uniforms (ViewPos, ShadowMap for PBR)
				if (s_CurrentRenderMode == RenderMode::Default && s_Camera) {
					currentShader->SetUniformVec3("u_ViewPos", s_Camera->GetPosition());
					s_ShadowMap->BindForReading(GL_TEXTURE4); // Assuming unit 4 for shadow map
					currentShader->SetUniformInt("shadowMap", 4);
					currentShader->SetUniformMat4("lightSpaceMatrix", s_ShadowMap->GetLightSpaceMatrix());
				}

				// Tick and Render the world using the selected forward shader
				s_World->Tick(delta);
				s_World->Render(*currentShader, view, s_CurrentRenderMode); // World::Render handles lights/materials for forward

				// Restore polygon mode if wireframe was used
				if (s_CurrentRenderMode == RenderMode::Wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

			} else { // s_CurrentRenderingPath == RenderingPath::Deferred
				// --- Deferred Shading Path ---

				// 1. G-Buffer Pass: Render geometry data
				s_GBufferFBO->Bind();
				glViewport(0, 0, display_w, display_h); // Ensure viewport matches G-Buffer size
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// Clear G-Buffer
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Always fill for G-Buffer

				s_GBufferShader->Bind();
				// Render opaque objects using GBuffer shader
				// Need a way to render geometry without material uniforms, maybe a new World::RenderGeometry method
				// Or modify StaticMeshComponent::Render/Model::Draw to accept a flag/shader type
				for (const auto &actor : s_World->GetActors()) {
					auto meshComp = actor->GetComponent<StaticMeshComponent>();
					if (meshComp) {
						// Call a method on StaticMeshComponent to render geometry for G-Buffer
						// This method needs to be added to StaticMeshComponent
						// It should handle both Model and Mesh cases internally
						meshComp->RenderGeometry(*s_GBufferShader); // Assuming RenderGeometry exists or will be added
					}
				}
				s_GBufferFBO->Unbind();

				// 2. Lighting Pass: Calculate lighting using G-Buffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to default buffer (or PostProcess HDR FBO)
				glClear(GL_COLOR_BUFFER_BIT);		  // Only clear color, depth is handled differently or not needed
				glDisable(GL_DEPTH_TEST);			  // No depth test for full-screen quad

				s_DeferredLightingShader->Bind();
				// Bind G-Buffer textures
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, s_GBufferFBO->GetColorAttachment(0)); // Position + Metallic
				s_DeferredLightingShader->SetUniformInt("gPositionMetallic", 0);   // Corrected name
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, s_GBufferFBO->GetColorAttachment(1)); // Normal + Roughness
				s_DeferredLightingShader->SetUniformInt("gNormalRoughness", 1);	   // Corrected name
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, s_GBufferFBO->GetColorAttachment(2)); // Albedo + AO
				s_DeferredLightingShader->SetUniformInt("gAlbedoAO", 2);		   // Corrected name

				// Bind Shadow Map
				s_ShadowMap->BindForReading(GL_TEXTURE4); // Assuming unit 4
				s_DeferredLightingShader->SetUniformInt("shadowMap", 4);
				s_DeferredLightingShader->SetUniformMat4("lightSpaceMatrix", s_ShadowMap->GetLightSpaceMatrix());

				// Set light uniforms (needs adaptation for deferred shader)
				// Example: Pass light data via UBO or uniform arrays
				// Set light uniforms (needs adaptation for deferred shader)
				s_DeferredLightingShader->SetUniformVec3("u_ViewPos", s_Camera->GetPosition());

				// --- Setup Light Uniforms for Deferred Shader ---
				// This part needs to be implemented based on how you structure lights in the deferred shader.
				// You'll likely iterate through lights similar to World::Render but set uniforms
				// matching the structures (e.g., u_PointLights, u_DirLights) in deferred_lighting.frag.

				// Example (needs full implementation):
				int pointLightCount					= 0;
				int dirLightCount					= 0;
				const int MAX_POINT_LIGHTS_DEFERRED = 10; // Match shader define
				const int MAX_DIR_LIGHTS_DEFERRED	= 1;  // Match shader define

				for (const auto &actor : s_World->GetActors()) {
					if (auto dirLight = actor->GetComponent<DirectionalLightComponent>()) {
						if (dirLightCount < MAX_DIR_LIGHTS_DEFERRED) {
							std::string baseName = "u_DirLights[" + std::to_string(dirLightCount) + "]";
							s_DeferredLightingShader->SetUniformVec3(baseName + ".direction", dirLight->GetDirection());
							s_DeferredLightingShader->SetUniformVec3(baseName + ".color", dirLight->GetColor());
							// Add intensity if your shader struct uses it
							dirLightCount++;
						}
					} else if (auto pointLight = actor->GetComponent<PointLightComponent>()) {
						// Exclude SpotLights if they are handled separately or not supported yet
						if (!dynamic_cast<SpotLightComponent *>(pointLight) && pointLightCount < MAX_POINT_LIGHTS_DEFERRED) {
							std::string baseName = "u_PointLights[" + std::to_string(pointLightCount) + "]";
							s_DeferredLightingShader->SetUniformVec3(baseName + ".position", pointLight->GetOwner()->GetRootComponent()->GetWorldPosition());
							s_DeferredLightingShader->SetUniformVec3(baseName + ".color", pointLight->GetColor());
							// Add intensity if your shader struct uses it
							s_DeferredLightingShader->SetUniformFloat(baseName + ".constant", pointLight->GetConstant());
							s_DeferredLightingShader->SetUniformFloat(baseName + ".linear", pointLight->GetLinear());
							s_DeferredLightingShader->SetUniformFloat(baseName + ".quadratic", pointLight->GetQuadratic());
							pointLightCount++;
						}
					}
					// Add SpotLight handling if needed
				}
				s_DeferredLightingShader->SetUniformInt("u_NumPointLights", pointLightCount);
				s_DeferredLightingShader->SetUniformInt("u_NumDirLights", dirLightCount);
				// --- End Light Uniform Setup ---

				renderQuad(); // Draw fullscreen quad to apply lighting

				// 3. Forward Pass (Transparency, Billboards, etc.)
				// Copy depth information from G-Buffer to default framebuffer
				glBindFramebuffer(GL_READ_FRAMEBUFFER, s_GBufferFBO->GetID()); // Use GetID() instead of GetFBO()
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);					   // Draw to default FBO
				glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default FBO

				glEnable(GL_DEPTH_TEST);
				// glDepthMask(GL_FALSE); // Optional: Render transparent objects without writing depth
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// Render billboards using a forward shader (e.g., PBR or Unlit)
				Shader *billboardShader = s_PBRShader; // Or s_UnlitShader
				billboardShader->Bind();
				// Set common uniforms again if needed (ViewPos, ShadowMap etc.)
				if (s_Camera) billboardShader->SetUniformVec3("u_ViewPos", s_Camera->GetPosition());
				// ... set other uniforms ...
				for (const auto &actor : s_World->GetActors()) {
					auto billboards = actor->GetComponentsByClass<BillboardComponent>();
					for (auto *billboard : billboards) {
						if (billboard) {
							// BillboardComponent::Render needs to handle the shader correctly
							billboard->Render(*billboardShader, view, RenderMode::Default); // Use appropriate mode
						}
					}
				}

				glDisable(GL_BLEND);
				// glDepthMask(GL_TRUE); // Restore depth writing if it was disabled
			}

			// --- Post Processing (If enabled, would happen here or wrap the main rendering) ---
			// s_PostProcessor->Render([&]() { /* Render logic goes here */ });

			// --- Swap Buffers & Poll Events ---
			glActiveTexture(GL_TEXTURE0); // Reset active texture unit
			glfwSwapBuffers(s_Window);
			glfwPollEvents();
		}
	}

	void Application::Shutdown() {
		// Unload plugins (DynamicModule dtor calls ShutdownFunction + dlclose)
		std::cout << "[INFO] Unloading plugins..." << std::endl;
		s_Plugins.clear();

		s_PostProcessor.reset(); // Release PostProcessor before other resources
		s_ShadowMap.reset();	 // Release ShadowMap
		s_DepthShader.reset();	 // Release Depth Shader
		delete s_World;
		delete s_UBO;
		// Delete all shaders
		delete s_PBRShader;
		delete s_UnlitShader;
		delete s_WireframeShader;
		delete s_Camera;

		s_GBufferFBO.reset();
		s_GBufferShader.reset();
		s_DeferredLightingShader.reset();

		s_PrimitiveMeshes.clear(); // Release primitive meshes
		glfwDestroyWindow(s_Window);
		glfwTerminate();
	}

} // namespace Engine