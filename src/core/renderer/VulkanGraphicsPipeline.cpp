#include "core/renderer/VulkanGraphicsPipeline.hpp"

#include "graphics/Shader.hpp"

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    : m_pipeline(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE) {}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

void VulkanGraphicsPipeline::createPipeline(VkDevice device)
{
    createPipelineLayout();
    createGraphicsPipeline(device);
}

void VulkanGraphicsPipeline::createPipelineLayout() {}

void VulkanGraphicsPipeline::createGraphicsPipeline(VkDevice device)
{
    const std::string vertFilePath = "assets/shaders/vertex/simple_shader.vert.spv"; // It is be in build
    const std::string fragFilePath = "assets/shaders/fragment/simple_shader.frag.spv";
    m_simpleShader = new Shader(device, vertFilePath, fragFilePath);
    if (m_simpleShader)
        m_device = device;
}

void VulkanGraphicsPipeline::cleanUp()
{
    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }

    if (m_simpleShader != nullptr)
    {
        delete m_simpleShader;
        m_simpleShader = nullptr;
    }
}
