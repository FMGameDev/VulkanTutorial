#include "core/renderer/VulkanRenderer.hpp"
#include "core/system/window/WindowHandler.hpp"

VulkanRenderer::VulkanRenderer(WindowHandler *windowHandler) : m_windowHandler(windowHandler)
{
}

VulkanRenderer::~VulkanRenderer()
{
    cleanup();
}

void VulkanRenderer::initVulkan()
{
    m_vulkanInstance.createInstance(m_vulkanValidationLayer);

    m_vulkanDebugMessenger.setup(m_vulkanInstance.getInstance());

    m_vulkanSurface.createMetalSurface(m_vulkanInstance.getInstance(), m_windowHandler->GetMetalLayer());

    m_vulkanDevice.pickPhysicalDevice(m_vulkanInstance.getInstance(), m_vulkanSurface.getSurface());

    m_vulkanDevice.createLogicalDevice(m_vulkanSurface.getSurface(), m_vulkanValidationLayer);

    int widthPx, heightPx;
    m_windowHandler->GetWindowFramebufferSize(&widthPx, &heightPx);
    m_vulkanSwapChain.createSwapChain(m_vulkanDevice.getDevice(), m_vulkanDevice.getPhysicalDevice(), m_vulkanSurface.getSurface(), widthPx, heightPx);

    m_vulkanSwapChain.createImageViews();
}

void VulkanRenderer::cleanup()
{
    // Vulkan components clean up
    m_vulkanSwapChain.cleanUp();

    m_vulkanDevice.cleanUp();

    m_vulkanSurface.cleanUp();

    if (m_vulkanValidationLayer.isValidationLayersEnabled())
    {
        m_vulkanDebugMessenger.cleanUp(m_vulkanInstance.getInstance());
    }

    m_vulkanInstance.cleanUp();
}