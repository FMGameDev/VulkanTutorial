cmake_minimum_required(VERSION 3.28)
project(VulkanTutorial VERSION 1.0.0)

# Define a variable for the target name
set(TARGET_NAME VulkanTutorial)

# Specify C++ standard at target level
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find third parties
find_package(glfw3 REQUIRED)
if(!glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found.")
endif()

find_package(Vulkan REQUIRED)
if(!Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found.")
endif()

# Source files
set(SRC_FILES
    src/main.cpp

    src/app/App.cpp

    src/core/Engine.cpp

    src/core/system/window/MacOsWindowUtils.mm
    src/core/system/window/WindowHandler.cpp

    src/core/renderer/VulkanDebugMessenger.cpp
    src/core/renderer/VulkanDevice.cpp
    src/core/renderer/VulkanGraphicsPipeline.cpp
    src/core/renderer/VulkanInstance.cpp
    src/core/renderer/VulkanRenderer.cpp
    src/core/renderer/VulkanSurface.cpp
    src/core/renderer/VulkanSwapChain.cpp
    src/core/renderer/VulkanValidationLayer.cpp
    
    src/utilities/logging/Logger.cpp
)

add_executable(${TARGET_NAME}
    ${SRC_FILES}
 )

 # Include directories
set(INCLUDE_DIRS 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/GLFW/include
    ${CMAKE_SOURCE_DIR}/third_party/VulkanSDK/include
    ${CMAKE_SOURCE_DIR}/third_party/VulkanSDK/include/vulkan
)

target_include_directories(${TARGET_NAME}
    PRIVATE
    ${INCLUDE_DIRS}
)

find_library(COCOA_FRAMEWORK Cocoa)
find_library(QUARTZCORE_FRAMEWORK QuartzCore)

# Link libraries
target_link_libraries(${TARGET_NAME}
    PRIVATE
    glfw
    Vulkan::Vulkan
    ${COCOA_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
)

# Optionally add a custom output directory for binaries
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)