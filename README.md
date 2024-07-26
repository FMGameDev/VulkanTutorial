# VulkanTutorial_Template

## Description
This project contains the basic set up for creating a vulkan project in MacOS for Apple Silicon and VulkanSDK1.3.290

## Pre-requeriments to use this project
The process to set up this project has been following the tutorial: https://vulkan-tutorial.com/Development_environment#page_MacOS with some changes.
- Installing VulkanSDK latest Mac version in "/Users/${USER}/VulkanSDK"
- Installing GLFW using homebrew. !IMPORTANT for Apple Silicone is installed in /opt/homebrew/Cellar/glfw not in /usr/local
- Installing GLM using homebrew

## XCode Settings done
- On Build Settings -> Library Search Paths
  - Header Search Paths added:
    - /Users/${USER}/VulkanSDK/macOS/include
    - /opt/homebrew/Cellar/glfw/3.4/include
  - Library Search Paths added:
    - /Users/${USER}/VulkanSDK/macOS/lib
    - /opt/homebrew/Cellar/glfw/3.4/lib

- On Build Phases
  - Link Binary With Libraries added files:
    - /opt/homebrew/Cellar/glfw/3.4/lib/libglfw.3.4.dylib
    - /Users/frdiaz/VulkanSDK/macOS/lib/libvulkan.1.3.290.dylib
    - /Users/frdiaz/VulkanSDK/macOS/lib/libvulkan.1.dylib
    - /Users/frdiaz/VulkanSDK/macOS/lib/libMoltenVK.dylib (!IMPORTANT: The tutorial does not include this, if this is not added it will not be able to locate this file)
  - Copy Files -> Destination Frameworks -> Subpath ""
    - Added the four past files from the framework list
   
- Product -> Scheme -> Edit Scheme -> Environment Variables Added:
  -   VK_ICD_FILENAMES=/Users/${USER}/VulkanSDK/macOS/share/vulkan/icd.d/MoltenVK_icd.json
  -   VK_LAYER_PATH=/Users/${USER}/VulkanSDK/macOS/share/vulkan/explicit_layer.d


WARNING: Make sure you have installed the pre-requeriments in the same location than this set up and you are using the same versions, if not updated the project settings followin your set up.
