# VintzGameEngine Roadmap

**Vision:**  
Créer un moteur de jeu moderne, modulaire et extensible, inspiré d'Unreal Engine, pour Linux, en C++17+, OpenGL 4.5, GLFW et GLAD.

**Quickstart:**  
- Code source : `src/`, headers publics : `include/`, dépendances : `third_party/`, exemples : `examples/`, shaders/assets : `assets/`
- Build : CMake (préféré), ou Makefile pour prototypage rapide

---

## Coding Guidelines
- C++17+, RAII, smart pointers, code modulaire (core, rendering, input, etc.)
- OpenGL 4.5 Core Profile, GLFW pour fenêtre/input, GLAD pour chargement OpenGL
- Documentation Doxygen pour API publiques
- Tests unitaires avec CTest

---

## 🧩 Phase 0 : Setup
- [x] Architecture du projet (`CMake`, `src/`, `assets/`, `include/`, `third_party/`)
- [x] Setup GLFW (création de fenêtre + input)
- [x] Setup GLAD (chargement OpenGL 4.5+)
- [x] Application + Renderer de base
- [x] Afficher un triangle avec shaders

## 📦 Phase 1 : Abstractions et fondations
- [x] Classe Shader
- [x] Classe VertexBuffer, IndexBuffer, VertexArray
- [x] Classe Texture
- [x] Classe Framebuffer
- [x] Classe Renderer (clear, draw, viewport, etc.)

## 🔷 Phase 2 : 3D Basics
- [x] Matrices : Projection (perspective), View (camera), Model (objets)
- [x] Création d’une Camera (freecam style FPS)
- [x] Uniform Buffer Objects (UBO) pour les matrices
- [x] Mesh de base (cube, plan, sphère hardcodé)

## 🌟 Phase 3 : Système Mesh / Model
- [x] Parser `.obj` (Assimp ou parser maison simple)
- [x] Classe Mesh et Model
- [x] Support Vertex Normals et UVs
- [x] Materials simples (diffuse)
- [x] Support Textures Diffuses

## 🔥 Phase 4 : Lumières réalistes (PBR Ready)
- [x] Directional Light
- [x] Point Lights
- [x] Spot Lights
- [x] Phong Lighting Model
- [x] Passage vers PBR (Physically Based Rendering)
  - [x] Metallic/Roughness Workflow
  - [x] Textures Albedo, Normal, Metallic, Roughness, AO
  - [x] Standard shader PBR

## 🧠 Phase 5 : Entity/Component System
- [x] Mini ECS (Entity Component System)
  - [x] Entity
  - [x] TransformComponent
  - [x] MeshRendererComponent
  - [x] LightComponent
- [x] Gestion position/rotation/scale par Entity

## 🖥️ Phase 6 : Framebuffer / Post-Processing
- [x] Framebuffer de rendu
- [x] Multiple Render Targets (MRT)
- [x] Bloom Effect
- [x] HDR Rendering
- [x] Tone Mapping (ACES Filmic, Reinhard, etc.)
- [x] Gamma Correction

## 🌈 Phase 7 : Physique et collisions
- [ ] Intégration Bullet Physics 3D
- [ ] Collider Components
- [ ] Rigidbody Components
- [ ] Simple Character Controller

## 🎮 Phase 8 : Input / Mouvements
- [ ] Mouse Picking (ray-cast)
- [ ] Contrôles clavier/souris
- [ ] Manipulateur d'objets 3D (translate, rotate, scale à la Unity)

## 📜 Phase 9 : Système de ressources
- [ ] Resource Manager (cache textures, meshes, shaders)
- [ ] Hot Reload des Shaders
- [ ] Gestion des Assets Paths (`assets/textures`, `assets/models`, ...)

## 🎨 Phase 10 : UI + Debug
- [ ] Intégration Dear ImGui
- [ ] UI Debug : stats CPU/GPU, frame rate, memory usage
- [ ] Explorer d'entités
- [ ] Modifications live dans la scène (transformations)

## 🌍 Phase 11 : Scène / Sauvegarde
- [ ] SceneManager
- [ ] Sauvegarde/Chargement de scènes (JSON/YAML)
- [ ] Gestion multi-scène
- [ ] Sérialisation Entities/Components

## 🛠️ Phase 12 : Optimisations
- [ ] Culling (Frustum Culling)
- [ ] LOD (Level of Detail)
- [ ] Batching des meshes
- [ ] Pipeline multithread (RenderThread, UpdateThread)

---

*Pour chaque phase, valider par un exemple dans `examples/` et des tests unitaires si pertinent.*