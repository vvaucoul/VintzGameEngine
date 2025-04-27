/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 11:19:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2025/04/27 12:10:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core/Application.h"
#include "Renderer/Camera.h"
#include "Renderer/MaterialPBR.h"
#include "Renderer/Model.h"
#include "Renderer/PostProcessor.h" // Make sure this is included
#include "Renderer/Primitives.h"
#include "Renderer/Shader.h"
#include "Renderer/ShadowMap.h" // Add include for ShadowMap
#include "Renderer/UniformBuffer.h"
#include "World/Actor.h"
#include "World/Components/DirectionalLightComponent.h" // Add include
#include "World/Components/LightComponent.h"			// Include base light component
#include "World/Components/PointLightComponent.h"		// Add include
#include "World/Components/SceneComponent.h"
#include "World/Components/SpotLightComponent.h" // Add include
#include "World/Components/StaticMeshComponent.h"
#include "World/World.h"

// Core Plugins
#include "CorePlugins/DynamicModule.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm> // Include algorithm for std::find_if
#include <iostream>	 // Add iostream for std::cerr and std::endl
#include <memory>	 // For std::unique_ptr
#include <stdexcept> // Include for std::exception
#include <vector>

namespace Engine {

	static GLFWwindow *s_Window							  = nullptr;
	static Camera *s_Camera								  = nullptr;
	static Shader *s_Shader								  = nullptr;
	static UniformBuffer *s_UBO							  = nullptr;
	static World *s_World								  = nullptr;
	static std::unique_ptr<PostProcessor> s_PostProcessor = nullptr; // Declare as static unique_ptr
	static std::unique_ptr<Engine::ShadowMap> s_ShadowMap = nullptr; // Moved declaration
	static std::unique_ptr<Engine::Shader> s_DepthShader  = nullptr; // Moved declaration

	static bool s_FirstMouse	 = true;
	static float s_LastX		 = 0.0f;
	static float s_LastY		 = 0.0f;
	static bool s_IsCameraActive = false; // Flag for RMB control

	// Store primitive meshes to keep them alive
	static std::vector<std::unique_ptr<Mesh>> s_PrimitiveMeshes;
	// Add: storage for loaded plugins
	static std::vector<std::unique_ptr<DynamicModule>> s_Plugins;

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
		glEnable(GL_MULTISAMPLE); // Enable MSAA

		// Camera
		s_Camera = new Camera({0.0f, 2.0f, 8.0f}, 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);

		// Shader + UBO - Load PBR Shaders
		// Update paths to Core directory
		s_Shader = new Shader("assets/shaders/Core/pbr.vert", "assets/shaders/Core/pbr.frag");
		s_UBO	 = new UniformBuffer(sizeof(glm::mat4) * 2, 0);

		// Initialize PostProcessor
		s_PostProcessor = std::make_unique<Engine::PostProcessor>(windowWidth, windowHeight);

		// Initialize ShadowMap
		s_ShadowMap = std::make_unique<Engine::ShadowMap>(2048, 2048);

		// Initialize Depth Shader
		// Update paths to Core directory
		s_DepthShader = std::make_unique<Engine::Shader>(
			"assets/shaders/Core/depth.vert",
			"assets/shaders/Core/depth.frag");

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
		// Dirt Material for the ground plane
		auto dirtMat = std::make_shared<Engine::MaterialPBR>();
		dirtMat->SetAlbedoMap("assets/textures/dirt/Dirt_Diffuse.png");
		dirtMat->SetNormalMap("assets/textures/dirt/Dirt_Normal.png");
		dirtMat->SetAOMap("assets/textures/dirt/Dirt_AmbientOcclusion.png");
		dirtMat->SetSpecularMap("assets/textures/dirt/Dirt_Specular.png");
		dirtMat->metallic  = 0.05f;
		dirtMat->roughness = 0.9f;

		// Simple Metallic Material for the sphere
		auto metalMat		  = std::make_shared<Engine::MaterialPBR>();
		metalMat->albedoColor = {0.8f, 0.8f, 0.85f}; // Light grey albedo - Corrected member name
		metalMat->metallic	  = 0.9f;
		metalMat->roughness	  = 0.2f;
		metalMat->ao		  = 1.0f;
		metalMat->specular	  = 0.5f; // Added specular property

		// Simple Plastic Material for cone/cylinder
		auto plasticMat			= std::make_shared<Engine::MaterialPBR>();
		plasticMat->albedoColor = {0.8f, 0.1f, 0.1f}; // Red albedo - Corrected member name
		plasticMat->metallic	= 0.05f;
		plasticMat->roughness	= 0.4f;
		plasticMat->ao			= 1.0f;

		// --- Actor Setup ---
		// Ground Plane (using Primitive) - Apply Dirt Material
		auto &planeActor = s_World->SpawnActor();
		planeActor.GetRootComponent()->SetPosition({0.0f, 0.0f, 0.0f});
		planeActor.GetRootComponent()->SetScale({10.0f, 1.0f, 10.0f}); // Make plane larger
		auto &planeMeshComp = planeActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[1].get());
		// planeMeshComp.SetMaterial(dirtMat);

		// Cube (using Model)
		auto &objActor = s_World->SpawnActor();
		objActor.GetRootComponent()->SetPosition({-3.0f, 0.5f, -2.0f});
		objActor.AddComponent<StaticMeshComponent>("assets/models/cube.obj");

		// Sphere (using Primitive) - Apply Metallic Material
		auto &sphereActor = s_World->SpawnActor();
		sphereActor.GetRootComponent()->SetPosition({0.0f, 0.75f, 0.0f});
		sphereActor.GetRootComponent()->SetScale({0.75f, 0.75f, 0.75f}); // Slightly smaller sphere
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
		coneMeshComp.SetMaterial(plasticMat); // Use same plastic material

		// Torus (using Primitive) - Default Material
		auto &torusActor = s_World->SpawnActor();
		torusActor.GetRootComponent()->SetPosition({2.0f, 0.5f, 2.0f});
		torusActor.GetRootComponent()->SetRotation({0.0f, 45.0f, 0.0f});
		torusActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[4].get());

		// Crate 1 (using Model)
		auto &crateActor1 = s_World->SpawnActor();
		crateActor1.GetRootComponent()->SetPosition({-2.5f, 0.5f, 0.5f});
		crateActor1.GetRootComponent()->SetRotation({0.0f, -30.0f, 0.0f});
		crateActor1.AddComponent<StaticMeshComponent>("assets/models/crate.obj");

		// Crate 2 (using Model)
		auto &crateActor2 = s_World->SpawnActor();
		crateActor2.GetRootComponent()->SetPosition({3.5f, 0.5f, 1.5f});
		crateActor2.GetRootComponent()->SetRotation({0.0f, 60.0f, 0.0f});
		crateActor2.GetRootComponent()->SetScale({1.2f, 1.2f, 1.2f}); // Slightly larger crate
		crateActor2.AddComponent<StaticMeshComponent>("assets/models/crate.obj");

		// Primitive Cube (using Primitive) - Default Material
		auto &primCubeActor = s_World->SpawnActor();
		primCubeActor.GetRootComponent()->SetPosition({0.5f, 0.5f, -3.0f});
		primCubeActor.AddComponent<StaticMeshComponent>(s_PrimitiveMeshes[5].get());

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
			float delta	  = current - lastFrame; // Use 'delta' consistently
			lastFrame	  = current;

			// Process keyboard input regardless of mouse state
			bool forward  = glfwGetKey(s_Window, GLFW_KEY_W) == GLFW_PRESS;
			bool backward = glfwGetKey(s_Window, GLFW_KEY_S) == GLFW_PRESS;
			bool left	  = glfwGetKey(s_Window, GLFW_KEY_A) == GLFW_PRESS;
			bool right	  = glfwGetKey(s_Window, GLFW_KEY_D) == GLFW_PRESS;
			bool up		  = glfwGetKey(s_Window, GLFW_KEY_E) == GLFW_PRESS;
			bool down	  = glfwGetKey(s_Window, GLFW_KEY_Q) == GLFW_PRESS;
			if (s_Camera) {
				s_Camera->ProcessKeyboard(delta, forward, backward, left, right, up, down);
			}
			// Note: Mouse input (rotation) is handled in MouseCallback, which already checks s_IsCameraActive

			// Clear buffers (moved inside PostProcessor::Render or done before it)
			// glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
			// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing is now handled by PostProcessor stages

			// Update UBO view & projection matrices
			glm::mat4 proj = s_Camera->GetProjectionMatrix();
			glm::mat4 view = s_Camera->GetViewMatrix();
			s_UBO->SetData(0, sizeof(glm::mat4), &proj[0][0]);
			s_UBO->SetData(sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]); // Offset by proj size

			// --- Shadow Mapping Pass ---
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

			glClearColor(0.05f, 0.05f, 0.08f, 1.0f); // Clear default framebuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST); // Ensure depth test is enabled

			s_Shader->Bind(); // Bind PBR shader
			if (s_Camera) {
				s_Shader->SetUniformVec3("u_ViewPos", s_Camera->GetPosition());
			}
			// UBO is already updated

			// Bind shadow map for reading
			s_ShadowMap->BindForReading(GL_TEXTURE4);
			s_Shader->SetUniformInt("shadowMap", 4);
			s_Shader->SetUniformMat4("lightSpaceMatrix", s_ShadowMap->GetLightSpaceMatrix());

			// Tick and Render the world using static variables
			s_World->Tick(delta);
			s_World->Render(*s_Shader); // Render world directly

			// Reset active texture unit (good practice)
			glActiveTexture(GL_TEXTURE0);

			glfwSwapBuffers(s_Window);
			glfwPollEvents();

			// exit(0);
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
		delete s_Shader;
		delete s_Camera;
		s_PrimitiveMeshes.clear(); // Release primitive meshes
		glfwDestroyWindow(s_Window);
		glfwTerminate();
	}

} // namespace Engine