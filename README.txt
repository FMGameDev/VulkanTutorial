# VulkanTutorial

## Description
This project is being done following the Vulkan Tutorial at https://vulkan-tutorial.com/ by Alexander Overvoorde Arpil 2023.
It has been generate from my Vulkan_Template project: https://github.com/FMGameDev/VulkanTutorial_Template where I have set up the xcode settings for VukanSDK in MacOS (Apple Silicone - Metal).
The latest commits are developed using Visual Studio Code.

## Vulkan Tutorial Indice (Track)
- Drawing a triangle
  - Setup (DONE)
    - Base Code (DONE)
    - Instance (DONE)
    - Validation layers (DONE)
    - Physical devices and queue families (DONE)
    - Logical deices and queues (DONE)
  - Presentation (DONE)
    - Window surface (DONE)
    - Swap chain (DONE)
    - Image views (DONE)
  - Graphics pipeline
    - Introduction
    - Shader modules
    - Fixed functions
    - Render passes
    - Conclusions
  - Drawing
    -  Framebuffers
    -  Command buffers
    -  Rendering and presentation
    -  Frames in flight
    -  Swap chain recreation
- ...

WARNING: Make sure you have installed the pre-requeriments in the same location than this set up and you are using the same versions, if not updated the project settings followin your set up.


## Project Structure:
It has been structured following best practise and industry standards to create a scalable project.

VulkanProject/
│
├── assets/               # Asset files such as shaders, textures, models, etc.
│   ├── audio/            # Audio files (e.g., .wav, .mp3)
│   ├── materials/        # Materials and related files
│   ├── models/           # 3D model files (e.g., .obj, .fbx)
│   ├── shaders/          # Shader files (e.g., vert,frag)
│   │   ├── vertex/
│   │   │   └── simple_shader.vert
│   │   ├── fragment/
│   │   │   └── simple_shader.frag
│   │   └── compute/
│   └── textures/         # Texture files (e.g., .png, .jpg)
│
├── build/                # Build output directory (ignored in version control)
│
├── docs/                 # Documentation
│
├── include/              # Project-wide headers
│   │
│   ├── app/        # Application-related headers
│   │   └── App.hpp     # Application-level components
│   │
│   ├── core/             # Core engine components
│   │   ├── events/          # Event handling
│   │   ├── renderer/             # Vulkan-specific or low-level rendering pipeline
│   │   │   ├── VulkanRenderer.hpp
│   │   │   ├── Pipeline.hpp
│   │   │   ├── VulkanInstance.hpp
│   │   │   ├── VulkanDevice.hpp
│   │   │   ├── VulkanSwapChain.hpp
│   │   │   └── VulkanGraphicsPipeline.hpp
│   │   ├── system/          # System-level components (e.g., timers, managers)
│   │   │    └── window/
│   │   │        ├── MacOsWindowUtils.hpp
│   │   │        └── WindowHandler.hpp
│   │   └──  Engine.hpp          # Central engine management
│   │
│   ├── graphics/             # Higher-levelgraphics abstractions or data structures (e.g., Mesh, Texture)
│   │
│   ├── input/                 # Input Handling
│   │
│   ├── scene/                 # Scene Management
│   │
│   └── utilities/                  # Utility implementations
│       ├── math/                       # Mathematical utilities
│       └── logging/                 # Logging utilities
│           └── Logger.hpp
│
├── src/                  # Source files
│   │
│   ├── app/        # Application-related code
│   │   └──  App.cpp     # Application-level components
│   │
│   ├── core/             # Core engine components
│   │   ├── events/          # Event handling
│   │   ├── renderer/             # Vulkan-specific components
│   │   │   ├── VulkanRenderer.cpp
│   │   │   ├── Pipeline.cpp
│   │   │   ├── VulkanInstance.cpp
│   │   │   ├── VulkanDevice.cpp
│   │   │   ├── VulkanSwapChain.cpp
│   │   │   └── VulkanGraphicsPipeline.cpp
│   │   ├── system/          # System-level components (e.g., timers, managers)
│   │   │   └── window/
│   │   │       ├── MacOsWindowUtils.cpp
│   │   │       └── WindowHandler.cpp
│   │   └──  Engine.cpp          # Central engine management
│   │
│   ├── graphics/             # Higher-levelgraphics abstractions or data structures (e.g., Mesh, Texture)
│   │
│   ├── input/                 # Input Handling
│   │
│   ├── scene/                 # Scene Management
│   │
│   ├── utilities/                  # Utility implementations
│   │   ├── math/                       # Mathematical utilities
│   │   └── logging/                 # Logging utilities
│   │       └── Logger.cpp
│   │
│   └──  main.cpp            # Entry point of the application
│
├── third_party/         # Third-party dependencies (Vulkan SDK, GLFW, etc.)
│   ├── VulkanSDK/          # Vulkan SDK
│   │   ├── include/            # Header files for Vulkan SDK
│   │   │   └── vulkan/         # Vulkan-specific headers
│   │   └── lib/                # Vulkan SDK libraries
│   │
│   └── GLFW/               # GLFW include and library files
│       ├── include/            # GLFW headers
│       └── lib/                # GLFW libraries
│
├── .gitignore
├── CMakeLists.txt        # Top-level CMake configuration file
└── README.md             # Project documentation
