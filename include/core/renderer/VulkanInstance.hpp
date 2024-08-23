#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanValidationLayer;

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance();

    void createInstance(const VulkanValidationLayer &vulkanValidationLayer);
    void cleanUp();

    VkInstance getInstance() const { return m_instance; };

private:
    VkInstance m_instance;

    std::vector<const char *> getRequiredGlfwExtensions(const bool isValidationLayersEnabled);
    bool checkGlfwExtensionsSupport(const std::vector<const char *> &glfwRequiredExtensions);
};
