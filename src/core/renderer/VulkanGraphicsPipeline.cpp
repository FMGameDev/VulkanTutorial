#include "core/renderer/VulkanGraphicsPipeline.hpp"

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    : m_pipeline(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE) {}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }
}

void VulkanGraphicsPipeline::createPipeline()
{
    createPipelineLayout();
    createGraphicsPipeline();
}

void VulkanGraphicsPipeline::createPipelineLayout() {}

void VulkanGraphicsPipeline::createGraphicsPipeline() {}

void VulkanGraphicsPipeline::cleanUp() {}
