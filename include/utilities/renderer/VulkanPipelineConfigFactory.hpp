#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <glfw3.h>

struct VulkanGraphicsPipelineConfig
{
    // Vertex input state configuration
    VkPipelineVertexInputStateCreateInfo m_vertexInputInfo{};

    // Input assembly state configuration
    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo{};

    // Viewport and scissor state configuration
    VkViewport m_viewport{};
    VkRect2D m_scissor{};
    VkPipelineViewportStateCreateInfo m_viewportInfo{};

    // Rasterization state configuration
    VkPipelineRasterizationStateCreateInfo m_rasterizationInfo{};

    // Multisample state configuration
    VkPipelineMultisampleStateCreateInfo m_multisampleInfo{};

    // Depth and stencil state configuration
    VkPipelineDepthStencilStateCreateInfo m_depthStencilInfo{};

    // Color blend state configuration
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo m_colorBlendInfo{};

    // Dynamic state configuration
    VkPipelineDynamicStateCreateInfo m_dynamicStateInfo{};
    std::vector<VkDynamicState> m_dynamicStates;

    // Tessellation
    VkPipelineTessellationStateCreateInfo m_tessellationInfo{};

    // For instanced rendering
    VkVertexInputBindingDescription vertexBindingDescription{};
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions{};
};

class VulkanPipelineConfigFactory
{
public:
    // Basic Pipeline Configuration. This is a minimal configuration for a basic graphics pipeline, suitable for rendering simple objects.
    static void basicPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Pipeline with Alpha Blending. This configuration is used when you need transparency and blending in your rendering.
    static void alphaBlendingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Wireframe Rendering. Useful for debugging or for special visual effects where the object’s wireframe is desired.
    static void wireframePipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Multisampled Anti-Aliasing (MSAA). For higher-quality rendering with anti-aliasing.
    static void msaaPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples);

    // Deferred Shading Pipeline. Used in more advanced rendering techniques, particularly in 3D graphics where lighting is calculated in a separate pass.
    static void deferredShadingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Pipeline with Dynamic States. This allows for changing certain pipeline states at runtime without recreating the entire pipeline.
    static void postProcessingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Post Processing.
    // For post-processing effects, which might not require depth testing or might use a different set of shader stages.
    static void dynamicStatePipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Shadow Mapping.
    // Used in shadow mapping techniques where you render the scene from the light's perspective to generate a shadow map.
    static void shadowMappingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D shadowMapExtent);

    // HDR (High Dynamic Range).
    // Render scenes in HDR, useful for post-processing passes where you tone-map the scene to SDR.
    static void hdrPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Instanced Rendering
    // Purpose: Optimize rendering of large numbers of identical objects (like grass, trees, or particles) by leveraging instancing.
    // Key Settings:
    // - Adjust vertex input to accommodate instance data.
    // - Ensure dynamic states are set if needed.
    // void instancedRenderingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Tessellation Pipeline Configuration
    // Purpose: Used when tessellation shaders are required, which are common in terrain rendering or other scenarios needing highly detailed surfaces.
    // Key Settings:
    // - Enable tessellation control and evaluation shaders.
    // - Adjust the pipeline layout to accommodate tessellation stages.
    void tessellationPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);

    // Depth Pre-Pass
    // Purpose: This technique renders only the depth information in a first pass to improve performance in complex scenes with heavy overdraw.
    // Key Settings:
    // - Render depth only in the first pass.
    // - Disable color output and configure the depth test to be VK_COMPARE_OP_LESS.
    void depthPrePassPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent);
};