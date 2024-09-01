#include "core/renderer/VulkanRenderer.hpp"
#include "core/system/window/WindowHandler.hpp"
#include "utilities/renderer/VulkanPipelineConfigFactory.hpp"
#include "graphics/Shader.hpp"

VulkanRenderer::VulkanRenderer(WindowHandler *windowHandler) : m_windowHandler(windowHandler), m_vulkanRenderPass(nullptr)
{
}

VulkanRenderer::~VulkanRenderer()
{
    cleanup();
}

void VulkanRenderer::initVulkan()
{
    // Create Instance
    m_vulkanInstance.createInstance(m_vulkanValidationLayer);

    // Set Up Debug Messenger
    m_vulkanDebugMessenger.setUp(m_vulkanInstance.getInstance());

    // Create Surface
    m_vulkanSurface.createMetalSurface(m_vulkanInstance.getInstance(), m_windowHandler->GetMetalLayer());

    // Pick Physical Device
    m_vulkanDevice.pickPhysicalDevice(m_vulkanInstance.getInstance(), m_vulkanSurface.getSurface());

    // Create Logical Device
    m_vulkanDevice.createLogicalDevice(m_vulkanSurface.getSurface(), m_vulkanValidationLayer);

    // Create SwapChain
    int widthPx, heightPx;
    m_windowHandler->GetWindowFramebufferSize(&widthPx, &heightPx);
    m_vulkanSwapChain.createSwapChain(m_vulkanDevice.getDevice(), m_vulkanDevice.getPhysicalDevice(), m_vulkanSurface.getSurface(), widthPx, heightPx);

    // Create Image Views
    m_vulkanSwapChain.createImageViews();

    // Create Render Pass
    m_vulkanRenderPass = VulkanRenderPass(m_vulkanDevice.getDevice());
    m_vulkanRenderPass.createRenderPass(m_vulkanSwapChain.getSwapChainFormat());

    // Create Graphics Pipeline
    VulkanGraphicsPipelineConfig pipelineConfigInfo{};
    VulkanPipelineConfigFactory::basicPipelineConfig(pipelineConfigInfo, m_vulkanSwapChain.getSwapChainExtent());
    const std::string vertFilePath = "assets/shaders/vertex/simple_shader.vert.spv";
    const std::string fragFilePath = "assets/shaders/fragment/simple_shader.frag.spv";
    Shader shader(m_vulkanDevice.getDevice(), vertFilePath, fragFilePath);
    m_vulkanGraphicsPipeline.createPipeline(m_vulkanDevice.getDevice(), pipelineConfigInfo, shader, m_vulkanRenderPass.getRenderPass());
}

// Vulkan components clean up
void VulkanRenderer::cleanup()
{
    m_vulkanSwapChain.cleanUp();

    m_vulkanGraphicsPipeline.cleanUp();

    m_vulkanRenderPass.cleanUp();

    m_vulkanDevice.cleanUp();

    m_vulkanSurface.cleanUp();

    if (m_vulkanValidationLayer.isValidationLayersEnabled())
    {
        m_vulkanDebugMessenger.cleanUp(m_vulkanInstance.getInstance());
    }

    m_vulkanInstance.cleanUp();
}