#pragma once

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanSurface.hpp"
#include "VulkanValidationLayer.hpp"

class WindowHandler;

class VulkanRenderer
{
public:
    VulkanRenderer(WindowHandler *windowHandler);
    ~VulkanRenderer();

    void initVulkan();
    void cleanup();

private:
    WindowHandler *m_windowHandler;

    VulkanDebugMessenger m_vulkanDebugMessenger;
    VulkanDevice m_vulkanDevice;
    VulkanGraphicsPipeline m_vulkanGraphicsPipeline;
    VulkanInstance m_vulkanInstance;
    VulkanSurface m_vulkanSurface;
    VulkanSwapChain m_vulkanSwapChain;
    VulkanValidationLayer m_vulkanValidationLayer;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};
