#pragma once

#include <vulkan/vulkan.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

class VulkanDebugMessenger
{
public:
    VulkanDebugMessenger();
    ~VulkanDebugMessenger();

    void setUp(const VkInstance &instance);
    void cleanUp(const VkInstance &instance);

private:
    VkDebugUtilsMessengerEXT m_debugMessenger;

    VkResult createDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pVulkanDebugMessenger);
    void destroyDebugUtilsMessengerEXT(const VkInstance &instance, VkDebugUtilsMessengerEXT VulkanDebugMessenger, const VkAllocationCallbacks *pAllocator);
};
