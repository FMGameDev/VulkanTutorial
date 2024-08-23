#include "core/renderer/VulkanDevice.hpp"

#include "core/renderer/VulkanSwapChain.hpp"
#include "core/renderer/VulkanValidationLayer.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <set>
#include <map>
#include <iostream>

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
{
    // Logic to find queue family to populate struct with
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.m_graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.m_presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

VulkanDevice::VulkanDevice() : m_device(VK_NULL_HANDLE) {}

VulkanDevice::~VulkanDevice() {}

void VulkanDevice::pickPhysicalDevice(const VkInstance &instance, const VkSurfaceKHR &surface)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto &physicalDevice : physicalDevices)
    {
        int score = rateDeviceSuitability(physicalDevice, surface);
        candidates.insert(std::make_pair(score, physicalDevice));
    }

    // Check if the best candidate is suitable
    if (candidates.rbegin()->first > 0)
    {
        m_physicalDevice = candidates.rbegin()->second;
    }
    else
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

void VulkanDevice::createLogicalDevice(const VkSurfaceKHR &surface, const VulkanValidationLayer &vulkanValidationLayer)
{
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.m_graphicsFamily.value(), indices.m_presentFamily.value()};

    // Priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0. Required
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    // Previous implementations of Vulkan made a distinction between instance and device specific validation layers but this is not longer the case
    // Tthe following condition will be ignored in up-to-date implementations however it has been added to make compatible with older implementations
    if (vulkanValidationLayer.isValidationLayersEnabled())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidationLayer.getValidationLayers().size());
        createInfo.ppEnabledLayerNames = vulkanValidationLayer.getValidationLayers().data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    // Instantiate the logical device
    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create logical device! VkResult: ") + string_VkResult(result));
    }

    vkGetDeviceQueue(m_device, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.m_presentFamily.value(), 0, &m_presentQueue);
}

int VulkanDevice::rateDeviceSuitability(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
{

    // Get basic device properties like the name, type and supported Vulkan version
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    // Get Support for optional features like texture compresion, 64 bit floats and multi viewport rendering(useful for VR)
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    // Mandatory Features and Properties //
    // if (!deviceFeatures.geometryShader) { // Note: Vulkan on Mac (MoltenVK) does not support Geometry Shader, it will return false even if this device supports geometry shaders
    //    return 0;
    //}
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    if (!indices.isComplete())
    {
        return 0;
    }

    const bool deviceExtensionsSupported = checkDeviceExtensionSupport(physicalDevice);
    if (!deviceExtensionsSupported)
    {
        return 0;
    }

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    if (!swapChainSupport.isAdequate())
    {
        return 0;
    }

    // Optional Features and Properties //
    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
        score += 500;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

bool VulkanDevice::checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice)
{
    uint32_t supportedDeviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedDeviceExtensions(supportedDeviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtensionCount, supportedDeviceExtensions.data());

    std::cout << "Supported device extension: " << supportedDeviceExtensions.size() << "\n";
    for (const auto &extension : supportedDeviceExtensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
    std::cout << "Required device extension: " << requiredExtensions.size() << "\n";
    for (const auto &extension : supportedDeviceExtensions)
    {
        if (requiredExtensions.erase(extension.extensionName))
        {
            std::cout << '\t' << "Required device extension '" << extension.extensionName << "' is supported." << '\n';
        }
    }

    for (const auto &extension : requiredExtensions)
    {
        std::cerr << '\t' << "Required device extension '" << extension << "' is not supported." << '\n';
    }

    std::cout << std::endl;

    return requiredExtensions.empty();
}

void VulkanDevice::cleanUp()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
}
