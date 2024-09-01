#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class VulkanGraphicsPipelineConfig;

class Shader;

class VulkanGraphicsPipeline
{
public:
    VulkanGraphicsPipeline();
    ~VulkanGraphicsPipeline();

    void createPipeline(const VkDevice &device, const VulkanGraphicsPipelineConfig &configInfo, const Shader &shader, const VkRenderPass &renderPass, const uint32_t subpass = 0);
    void cleanUp();

    VkPipeline getPipeline() const { return m_graphicsPipeline; };
    VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; };

private:
    VkDevice m_device;
    VkPipeline m_graphicsPipeline;
    VkPipelineLayout m_pipelineLayout;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    void createPipelineLayout();
    void createGraphicsPipeline(const VulkanGraphicsPipelineConfig &configInfo, const VkRenderPass &renderPass, const uint32_t subpass);
    void setShaderStages(const Shader &shader);
};
