#pragma once

#include <vulkan/vulkan.h>

class Shader;

class VulkanGraphicsPipeline
{
public:
    VulkanGraphicsPipeline();
    ~VulkanGraphicsPipeline();

    void createPipeline(VkDevice device);
    void cleanUp();

    VkPipeline getPipeline() const { return m_pipeline; };
    VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; };

private:
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    VkDevice m_device;
    VkRenderPass m_renderPass;
    Shader *m_simpleShader;

    void createPipelineLayout();
    void createGraphicsPipeline(VkDevice device);
};
