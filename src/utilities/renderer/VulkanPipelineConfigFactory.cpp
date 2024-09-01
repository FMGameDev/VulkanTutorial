#include "utilities/renderer/VulkanPipelineConfigFactory.hpp"

void VulkanPipelineConfigFactory::basicPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    // Vertex Input
    configInfo.m_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    configInfo.m_vertexInputInfo.vertexBindingDescriptionCount = 0;
    configInfo.m_vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    configInfo.m_vertexInputInfo.vertexAttributeDescriptionCount = 0;
    configInfo.m_vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    // Input Assembly
    configInfo.m_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.m_inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.m_inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Viewport and Scissor
    configInfo.m_viewport = {};
    configInfo.m_viewport.x = 0.0f;
    configInfo.m_viewport.y = 0.0f;
    configInfo.m_viewport.width = static_cast<float>(swapChainExtent.width);
    configInfo.m_viewport.height = static_cast<float>(swapChainExtent.height);
    configInfo.m_viewport.minDepth = 0.0f;
    configInfo.m_viewport.maxDepth = 1.0f;

    configInfo.m_scissor = {};
    configInfo.m_scissor.offset = {0, 0};
    configInfo.m_scissor.extent = swapChainExtent;

    configInfo.m_viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.m_viewportInfo.viewportCount = 1;
    configInfo.m_viewportInfo.pViewports = &configInfo.m_viewport;
    // configInfo.m_viewportInfo.pViewports = nullptr; // If using dynamic state
    configInfo.m_viewportInfo.scissorCount = 1;
    configInfo.m_viewportInfo.pScissors = &configInfo.m_scissor;
    // configInfo.m_viewportInfo.pScissors = nullptr; // If using dynamic state

    // Rasterization
    configInfo.m_rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.m_rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.m_rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.m_rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.m_rasterizationInfo.lineWidth = 1.0f;
    configInfo.m_rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    configInfo.m_rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.m_rasterizationInfo.depthBiasEnable = VK_FALSE;

    // Multisample
    configInfo.m_multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.m_multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.m_multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    // configInfo.m_multisampleInfo.minSampleShading = 1.0f;          // Optional
    // configInfo.m_multisampleInfo.pSampleMask = nullptr;            // Optional
    // configInfo.m_multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    // configInfo.m_multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

    // Depth Stencil
    configInfo.m_depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.m_depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.m_depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.m_depthStencilInfo.stencilTestEnable = VK_FALSE;

    // Color Blend
    configInfo.m_colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.m_colorBlendAttachment.blendEnable = VK_FALSE;
    // configInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    // configInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    // configInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    // configInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    // configInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    // configInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

    configInfo.m_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.m_colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.m_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    configInfo.m_colorBlendInfo.attachmentCount = 1;
    configInfo.m_colorBlendInfo.pAttachments = &configInfo.m_colorBlendAttachment;

    // Dynamic State
    configInfo.m_dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    configInfo.m_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.m_dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.m_dynamicStates.size());
    configInfo.m_dynamicStateInfo.pDynamicStates = configInfo.m_dynamicStates.data();

    // Pipeline Layout and Render Pass (to be set later)
    // configInfo.m_pipelineLayout = VK_NULL_HANDLE;
    // configInfo.m_renderPass = VK_NULL_HANDLE;
    // configInfo.m_subpass = 0;
}

void VulkanPipelineConfigFactory::alphaBlendingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    // Basic config
    basicPipelineConfig(configInfo, swapChainExtent);

    // Enable blending
    configInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
    configInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    configInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void VulkanPipelineConfigFactory::wireframePipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    // Basic config
    basicPipelineConfig(configInfo, swapChainExtent);

    // Enable wireframe mode
    configInfo.m_rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
}

void VulkanPipelineConfigFactory::msaaPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples)
{
    // Basic config
    basicPipelineConfig(configInfo, swapChainExtent);

    // Set multisample settings
    configInfo.m_multisampleInfo.rasterizationSamples = msaaSamples;
    configInfo.m_multisampleInfo.sampleShadingEnable = VK_TRUE;
    configInfo.m_multisampleInfo.minSampleShading = 0.2f; // Optional, adjust based on performance needs
}

void VulkanPipelineConfigFactory::deferredShadingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    // Basic config
    basicPipelineConfig(configInfo, swapChainExtent);

    // Adjust settings for deferred shading
    configInfo.m_colorBlendAttachment.blendEnable = VK_FALSE; // Typically no blending in G-Buffer creation
    configInfo.m_depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthTestEnable = VK_TRUE;
    // Add more adjustments as needed for the G-Buffer pass
}

void VulkanPipelineConfigFactory::dynamicStatePipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    // Basic config
    basicPipelineConfig(configInfo, swapChainExtent);

    // Set dynamic states
    configInfo.m_dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH};
    configInfo.m_dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.m_dynamicStates.size());
    configInfo.m_dynamicStateInfo.pDynamicStates = configInfo.m_dynamicStates.data();
}

void VulkanPipelineConfigFactory::postProcessingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    basicPipelineConfig(configInfo, swapChainExtent);

    // Disable Depth Test for Post Processing
    configInfo.m_depthStencilInfo.depthTestEnable = VK_FALSE;
    configInfo.m_depthStencilInfo.depthWriteEnable = VK_FALSE;

    // Blending for Post Processing
    configInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
    configInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

    configInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void VulkanPipelineConfigFactory::shadowMappingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D shadowMapExtent)
{
    basicPipelineConfig(configInfo, shadowMapExtent);

    configInfo.m_rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT; // Cull front faces
    configInfo.m_depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    configInfo.m_colorBlendAttachment.blendEnable = VK_FALSE; // No color output for shadow map
}

void VulkanPipelineConfigFactory::hdrPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    basicPipelineConfig(configInfo, swapChainExtent);

    // Assuming HDR color format is used, set appropriate color blend settings
    configInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
    configInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    configInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    configInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

    configInfo.m_colorBlendInfo.logicOpEnable = VK_FALSE;
}

/*void VulkanPipelineConfigFactory::instancedRenderingPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    basicPipelineConfig(configInfo, swapChainExtent);

    // Customize vertex input for instanced data
    // Assuming you have instance-specific attributes (e.g., positions, colors)
    VkVertexInputBindingDescription instanceBinding = {};
    instanceBinding.binding = 1;                   // Instance data binding
    instanceBinding.stride = sizeof(InstanceData); // Instance-specific data
    instanceBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    configInfo.m_vertexInputInfo.vertexBindingDescriptionCount = 2;
    configInfo.m_vertexInputInfo.pVertexBindingDescriptions = new VkVertexInputBindingDescription[2]{configInfo.m_bindingDescription, instanceBinding};

    // Dynamic state example
    configInfo.m_dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    configInfo.m_dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.m_dynamicStates.size());
    configInfo.m_dynamicStateInfo.pDynamicStates = configInfo.m_dynamicStates.data();
}*/

void VulkanPipelineConfigFactory::tessellationPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    basicPipelineConfig(configInfo, swapChainExtent);

    // Enable tessellation
    configInfo.m_tessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    configInfo.m_tessellationInfo.patchControlPoints = 3; // Example: 3 control points for a triangle patch

    configInfo.m_rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;

    // Make sure the shader stages include tessellation control and evaluation shaders
    // This would be set when creating the pipeline with shader stages
}

void VulkanPipelineConfigFactory::depthPrePassPipelineConfig(VulkanGraphicsPipelineConfig &configInfo, VkExtent2D swapChainExtent)
{
    basicPipelineConfig(configInfo, swapChainExtent);

    configInfo.m_rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    configInfo.m_colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.m_depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.m_depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
}
