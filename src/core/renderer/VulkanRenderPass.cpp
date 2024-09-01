#include "core/renderer/VulkanRenderPass.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <vector>

VulkanRenderPass::VulkanRenderPass(const VkDevice &device)
    : m_device(device), m_renderPass(VK_NULL_HANDLE) {}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::createRenderPass(VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples)
{
    std::vector<VkAttachmentDescription> attachments;

    // Create color attachment
    VkAttachmentDescription colorAttachment = createColorAttachment(colorFormat, samples);
    attachments.push_back(colorAttachment);

    // Create depth attachment if depthFormat is provided
    bool hasDepthAttachment = (depthFormat != VK_FORMAT_UNDEFINED);
    if (hasDepthAttachment)
    {
        VkAttachmentDescription depthAttachment = createDepthAttachment(depthFormat, samples);
        attachments.push_back(depthAttachment);
    }

    // Subpass
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    if (hasDepthAttachment)
    {
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    if (hasDepthAttachment)
    {
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
    }

    // Render pass info
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkResult result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create render pass! VkResult: ") + string_VkResult(result));
    }
}

VkAttachmentDescription VulkanRenderPass::createColorAttachment(VkFormat format, VkSampleCountFlagBits samples) const
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = samples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;              // Clear the values to a constant at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;            // Rendered contents will be stored in memory and can be read later
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   // Existing contents are undefined; we don't care about them
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Contents of the framebuffer will be undefined after the rendering operation
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Images to be presented in the swap chain
    return colorAttachment;
}

VkAttachmentDescription VulkanRenderPass::createDepthAttachment(VkFormat format, VkSampleCountFlagBits samples) const
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = format;
    depthAttachment.samples = samples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return depthAttachment;
}

void VulkanRenderPass::cleanUp()
{
    if (m_renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }
}
