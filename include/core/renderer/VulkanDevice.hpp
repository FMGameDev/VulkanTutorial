#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices
{
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;
    bool isComplete()
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

class VulkanValidationLayer;

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();

    void pickPhysicalDevice(const VkInstance &instance, const VkSurfaceKHR &surface);
    void createLogicalDevice(const VkSurfaceKHR &surface, const VulkanValidationLayer &vulkanValidationLayer);

    void cleanUp();

    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    VkDevice getDevice() const { return m_device; }

private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;

    // Queue Family
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    const std::vector<const char *> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined(__APPLE__) && defined(__arm64__)
        "VK_KHR_portability_subset",
#endif
    };

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    int rateDeviceSuitability(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
};
