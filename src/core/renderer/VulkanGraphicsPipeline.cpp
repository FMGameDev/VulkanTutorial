#include "core/renderer/VulkanGraphicsPipeline.hpp"

#include "graphics/Shader.hpp"
#include "utilities/renderer/VulkanPipelineConfigFactory.hpp"

#include <vulkan/vk_enum_string_helper.h>

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    : m_graphicsPipeline(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE) {}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

void VulkanGraphicsPipeline::createPipeline(const VkDevice &device, const VulkanGraphicsPipelineConfig &configInfo, const Shader &shader, const VkRenderPass &renderPass, const uint32_t subpass)
{
    m_device = device;
    createPipelineLayout();
    setShaderStages(shader);
    createGraphicsPipeline(configInfo, renderPass, subpass);
}

void VulkanGraphicsPipeline::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create pipeline layout! VkResult: ") + string_VkResult(result));
    }
}

void VulkanGraphicsPipeline::createGraphicsPipeline(const VulkanGraphicsPipelineConfig &configInfo, const VkRenderPass &renderPass, const uint32_t subpass)
{
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &configInfo.m_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.m_inputAssemblyInfo;
    pipelineInfo.pViewportState = &configInfo.m_viewportInfo;
    pipelineInfo.pRasterizationState = &configInfo.m_rasterizationInfo;
    pipelineInfo.pMultisampleState = &configInfo.m_multisampleInfo;
    pipelineInfo.pColorBlendState = &configInfo.m_colorBlendInfo;
    pipelineInfo.pDepthStencilState = &configInfo.m_depthStencilInfo;
    pipelineInfo.pDynamicState = &configInfo.m_dynamicStateInfo;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;

    VkResult result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create graphics pipeline! VkResult: ") + string_VkResult(result));
    }
}

void VulkanGraphicsPipeline::setShaderStages(const Shader &shader)
{
    m_shaderStages.push_back(shader.getVertexShaderStageInfo());
    m_shaderStages.push_back(shader.getFragmentShaderStageInfo());
}

void VulkanGraphicsPipeline::cleanUp()
{
    if (m_graphicsPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
        m_graphicsPipeline = VK_NULL_HANDLE;
    }
    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}