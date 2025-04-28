# VintzGameEngine

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
<!-- Add other badges here: Build Status, etc. -->

**VintzGameEngine** is a modern C++17 game engine built from the ground up, inspired by the architecture and API design of Unreal Engine. It focuses on Linux compatibility and leverages modern graphics techniques using OpenGL 4.5 Core Profile.

## Vision

The goal is to create a modular, extensible, and high-performance game engine suitable for developing 3D applications and games on Linux platforms. Key technologies include C++17, OpenGL 4.5+, GLFW, GLAD, GLM, and Assimp.

## ✨ Features (Current & Planned)

*   **Modern C++17:** Utilizes `constexpr`, `enum class`, smart pointers, RAII, and other modern C++ features.
*   **Core Systems:** Robust `Application` loop, windowing and input via GLFW.
*   **Rendering Pipeline:**
    *   OpenGL 4.5 Core Profile via GLAD.
    *   Abstraction layer for GPU resources: `Shader`, `VertexBuffer`, `IndexBuffer`, `VertexArray`, `Texture`, `Framebuffer`, `UniformBuffer`.
    *   Physically Based Rendering (PBR) pipeline (Metallic/Roughness).
    *   Support for Directional, Point, and Spot lights.
    *   HDR Rendering with Tone Mapping (ACES Filmic, Reinhard) and Gamma Correction.
    *   Post-Processing Effects (Bloom).
*   **World/Scene:**
    *   Unreal-inspired `World` -> `Actor` -> `Component` hierarchy.
    *   `Actor` system with unique IDs.
    *   `SceneComponent` for hierarchical transformations (Position, Rotation, Scale).
    *   `StaticMeshComponent` for rendering geometry.
    *   Light Components (`DirectionalLightComponent`, `PointLightComponent`, `SpotLightComponent`).
*   **Asset Management:**
    *   Model loading via Assimp (supports `.obj` and potentially other formats).
    *   Texture loading via `stb_image`.
*   **Camera:** FPS-style free-look camera.
*   **Build System:** CMake (preferred) and Makefile support.

*(Check the [Roadmap.md](./Roadmap.md) for a detailed breakdown of completed and upcoming features)*

## 📸 Screenshots

Here are some glimpses of the engine in action:

| Default Scene                                                                                                | Wireframe View                                                                                               | PBR Materials                                                                                                |
| :-----------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------: | :-----------------------------------------------------------------------------------------------------------: |
| ![Default Scene](https://github.com/user-attachments/assets/4352e6ce-56e2-420d-8c91-44eb75b20345)             | ![Wireframe View](https://github.com/user-attachments/assets/ae4cbece-108d-4cce-9cbf-3edbb5b7105b)           | ![PBR Materials](https://github.com/user-attachments/assets/9cbdcb2c-d778-4fe3-82f8-62841b44a2b5)            |

## 🚀 Getting Started

### Prerequisites

*   C++17 compatible compiler (GCC >= 7 or Clang >= 5)
*   CMake (>= 3.10 recommended)
*   OpenGL 4.5 capable GPU and drivers
*   Development libraries for:
    *   GLFW3 (`libglfw3-dev` on Debian/Ubuntu)
    *   GLM (`libglm-dev` on Debian/Ubuntu)
    *   Assimp (`libassimp-dev` on Debian/Ubuntu)
    *   OpenGL (`libgl1-mesa-dev` or vendor-specific drivers)
    *   X11 libraries (usually installed with graphics drivers/GLFW)

### Building

**Using CMake (Recommended):**

```bash
git clone https://github.com/vvaucoul/VintzGameEngine.git
cd VintzGameEngine
mkdir build && cd build
cmake ..
make -j$(nproc)
```

**Using Makefile (For quick prototyping):**

```bash
git clone https://github.com/vvaucoul/VintzGameEngine.git
cd VintzGameEngine
make -j$(nproc)
```

### Running

The executable will be generated in the `bin/` directory (Makefile) or the build directory root (CMake).

```bash
# With Makefile
./bin/VintzGameEngine

# With CMake
./VintzGameEngine
```

*(Examples will be added later in the `examples/` directory)*

## 📁 Project Structure

```
/Engine/        # Core engine source code (Core, Renderer, World modules)
/include/       # Public engine headers (potentially for game projects using the engine)
/third_party/   # External libraries (GLAD, stb_image - others linked via system)
/assets/        # Default location for shaders, textures, models
/examples/      # Demo applications showcasing engine features (Planned)
/build/         # Build output directory (CMake/Makefile)
/bin/           # Executable output directory (Makefile)
CMakeLists.txt  # Main CMake build script
Makefile        # Alternative GNU Make build script
Readme.md       # This file
Roadmap.md      # Project development phases
LICENSE         # Project license (GPLv3)
.gitignore      # Git ignore rules
```

## 📜 Coding Guidelines

*   **C++ Standard:** C++17 or later.
*   **Style:** Follow Unreal Engine naming conventions (PascalCase for types, camelCase for functions/variables). Embrace modern C++ idioms (RAII, smart pointers, `auto`, `constexpr`, `enum class`).
*   **Memory Management:** Avoid raw `new`/`delete`; use `std::unique_ptr` and `std::shared_ptr`.
*   **Containers:** Prefer `std::vector` or `std::span` over raw C-style arrays.
*   **Documentation:** Use Doxygen comments for public APIs (`///` or `/** */`).
*   **Architecture:** Strictly follow the World -> Actor -> Component hierarchy.

## 🗺️ Roadmap

Development is planned in phases. See the [Roadmap.md](./Roadmap.md) file for detailed feature tracking and future plans.

## 🤝 Contributing

Contributions are welcome! If you'd like to contribute, please:

1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/YourFeature`).
3.  Make your changes, adhering to the coding guidelines.
4.  Commit your changes (`git commit -m 'Add some feature'`).
5.  Push to the branch (`git push origin feature/YourFeature`).
6.  Open a Pull Request.

Please consider opening an issue first to discuss any significant changes.

## 📄 License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](./LICENSE) file for full details.

## 🙏 Acknowledgements

This project utilizes several excellent open-source libraries:

*   [GLFW](https://www.glfw.org/): For window creation and input handling.
*   [GLAD](https://glad.dav1d.de/): For loading OpenGL function pointers.
*   [GLM](https://glm.g-truc.net/0.9.9/index.html): For OpenGL mathematics.
*   [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h): For simple image loading.
*   [Assimp](https://www.assimp.org/): For loading various 3D model formats.

The engine's architecture is heavily inspired by [Unreal Engine](https://www.unrealengine.com/).

