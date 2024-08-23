#include "core/renderer/VulkanInstance.hpp"

#include "core/renderer/VulkanDebugMessenger.hpp"
#include "core/renderer/VulkanValidationLayer.hpp"

#define VK_USE_PLATFORM_MACOS_MVK
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_metal.h> // Include for VK_EXT_metal_surface
#include <iostream>

VulkanInstance::VulkanInstance() : m_instance(VK_NULL_HANDLE) {}

VulkanInstance::~VulkanInstance() {}

void VulkanInstance::createInstance(const VulkanValidationLayer &vulkanValidationLayer)
{
    if (vulkanValidationLayer.isValidationLayersEnabled() && !vulkanValidationLayer.checkValidationLayersSupport())
    {
        throw std::runtime_error("Some of the validation layers requested were not available!");
    }

    // struct with information about our application (optional)
    // it may provide useful information to the driver for optimization
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Tells Vulkan driver which global extentions and validation layers we want to use (mandatory)
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

// Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
// Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
#if defined __APPLE__ && defined __arm64__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    std::vector<const char *> requiredExtensions = getRequiredGlfwExtensions(vulkanValidationLayer.isValidationLayersEnabled());
    createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vulkanValidationLayer.isValidationLayersEnabled())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayer.getValidationLayers().size());
        createInfo.ppEnabledLayerNames = vulkanValidationLayer.getValidationLayers().data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create instance! VkResult: ") + string_VkResult(result));
    }
}

std::vector<const char *> VulkanInstance::getRequiredGlfwExtensions(const bool isValidationLayersEnabled)
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
    // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
    std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if defined __APPLE__ && defined __arm64__
    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    if (isValidationLayersEnabled)
    {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        // Adding KHR surface extension and MVK_MACOS_SURFACE_EXTENSION
        requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);       // necessary for creating a Vulkan surface in a platform-independent way
        requiredExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME); // modern and supported way to create a Vulkan surface on macOS, leveraging Metal for rendering.
    }

    // Check if all the required extensions are supported
    if (!checkGlfwExtensionsSupport(requiredExtensions))
    {
        throw std::runtime_error("Failed to create instance, some of the required extensions were not supported.");
    }

    return requiredExtensions;
}

bool VulkanInstance::checkGlfwExtensionsSupport(const std::vector<const char *> &glfwRequiredExtensions)
{
    // Retrieve a list of supported extensions
    uint32_t supportedExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr); // Get how many extensions there are

    std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

    std::cout << "Supported GLFW extensions: " << supportedExtensions.size() << "\n";
    for (const auto &extension : supportedExtensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    // Proceed to check
    std::cout << "Required GLFW extensions: " << glfwRequiredExtensions.size() << "\n";
    bool allSupported = std::all_of(
        glfwRequiredExtensions.begin(),
        glfwRequiredExtensions.end(),
        [&supportedExtensions](const char *requiredExtension)
        {
            bool supported = std::any_of(
                supportedExtensions.begin(),
                supportedExtensions.end(),
                [requiredExtension](const VkExtensionProperties &ext)
                {
                    return strcmp(ext.extensionName, requiredExtension) == 0;
                });
            if (!supported)
            {
                std::cerr << '\t' << "Required GLFW extension '" << requiredExtension << "' is not supported." << '\n';
            }
            else
            {
                std::cout << '\t' << "Required GLFW extension '" << requiredExtension << "' is supported." << '\n';
            }

            return supported;
        });

    std::cout << std::endl;

    return allSupported;
}

void VulkanInstance::cleanUp()
{
    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}
