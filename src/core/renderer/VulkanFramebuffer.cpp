#include "core/renderer/VulkanFramebuffer.hpp"

#include <vulkan/vk_enum_string_helper.h>

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent, uint32_t layers)
    : m_device(device), m_extent(extent), m_framebuffer(VK_NULL_HANDLE)
{
    createFramebuffer(renderPass, attachments, extent, layers);
}

VulkanFramebuffer::~VulkanFramebuffer()
{
}

// Move constructor
VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer &&other) noexcept
    : m_device(other.m_device), m_framebuffer(other.m_framebuffer), m_extent(other.m_extent)
{
    other.m_framebuffer = VK_NULL_HANDLE;
}

// Move assignment operator
VulkanFramebuffer &VulkanFramebuffer::operator=(VulkanFramebuffer &&other) noexcept
{
    if (this != &other)
    {
        cleanUp();
        m_device = other.m_device;
        m_framebuffer = other.m_framebuffer;
        m_extent = other.m_extent;

        other.m_framebuffer = VK_NULL_HANDLE;
    }
    return *this;
}

void VulkanFramebuffer::createFramebuffer(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent, uint32_t layers)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = layers;

    VkResult result = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create Framebuffer! VkResult: ") + string_VkResult(result));
    }
}

void VulkanFramebuffer::resize(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D newExtent, uint32_t layers)
{
    if (newExtent.width == m_extent.width && newExtent.height == m_extent.height)
    {
        return; // No need to resize if the extent hasn't changed
    }
    cleanUp();
    m_extent = newExtent;
    createFramebuffer(renderPass, attachments, newExtent, layers);
}

void VulkanFramebuffer::cleanUp()
{
    if (m_framebuffer != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
    }
}