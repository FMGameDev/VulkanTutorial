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
    const VkInstance &instance = m_vulkanInstance.getInstance();

    // Set Up Debug Messenger
    m_vulkanDebugMessenger.setUp(instance);

    // Create Surface
    m_vulkanSurface.createMetalSurface(instance, m_windowHandler->GetMetalLayer());
    const VkSurfaceKHR &surface = m_vulkanSurface.getSurface();

    // Pick Physical Device
    m_vulkanDevice.pickPhysicalDevice(instance, surface);

    // Create Logical Device
    m_vulkanDevice.createLogicalDevice(surface, m_vulkanValidationLayer);
    const VkDevice &device = m_vulkanDevice.getDevice();

    // Create SwapChain
    int widthPx, heightPx;
    m_windowHandler->GetWindowFramebufferSize(&widthPx, &heightPx);
    m_vulkanSwapChain.createSwapChain(device, m_vulkanDevice.getPhysicalDevice(), surface, widthPx, heightPx);

    // Create Image Views
    m_vulkanSwapChain.createImageViews();
    const std::vector<VkImageView> &swapChainImageViews = m_vulkanSwapChain.getSwapChainImageViews();
    const VkFormat &swapChainImageFormat = m_vulkanSwapChain.getSwapChainFormat();
    const VkExtent2D &swapChainExtent = m_vulkanSwapChain.getSwapChainExtent();

    // Create Render Pass
    m_vulkanRenderPass = VulkanRenderPass(device);
    m_vulkanRenderPass.createRenderPass(swapChainImageFormat);
    const VkRenderPass &renderPass = m_vulkanRenderPass.getRenderPass();

    // Create Graphics Pipeline
    VulkanGraphicsPipelineConfig pipelineConfigInfo{};
    VulkanPipelineConfigFactory::basicPipelineConfig(pipelineConfigInfo, swapChainExtent);
    const std::string vertFilePath = "assets/shaders/vertex/simple_shader.vert.spv";
    const std::string fragFilePath = "assets/shaders/fragment/simple_shader.frag.spv";
    Shader shader(device, vertFilePath, fragFilePath);
    m_vulkanGraphicsPipeline.createPipeline(device, pipelineConfigInfo, shader, renderPass);

    // Create Framebuffers for swapChain
    m_vulkanSwapChainFramebuffers.reserve(swapChainImageViews.size());
    uint32_t layers = 1;

    for (const auto &imageView : swapChainImageViews)
    {
        m_vulkanSwapChainFramebuffers.emplace_back(VulkanFramebuffer(device, renderPass, std::vector<VkImageView>{imageView}, swapChainExtent, layers));
    }
}

// Vulkan components clean up
void VulkanRenderer::cleanup()
{
    for (VulkanFramebuffer &framebuffer : m_vulkanSwapChainFramebuffers)
    {
        framebuffer.cleanUp();
    }
    m_vulkanSwapChainFramebuffers.clear();

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