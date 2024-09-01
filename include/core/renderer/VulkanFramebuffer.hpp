#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent, uint32_t layers = 1);
    ~VulkanFramebuffer();

    // Prevent copying
    VulkanFramebuffer(const VulkanFramebuffer &) = delete;
    VulkanFramebuffer &operator=(const VulkanFramebuffer &) = delete;

    // Allow moving
    VulkanFramebuffer(VulkanFramebuffer &&other) noexcept;
    VulkanFramebuffer &operator=(VulkanFramebuffer &&other) noexcept;

    VkFramebuffer getFramebuffer() const { return m_framebuffer; }
    VkExtent2D getExtent() const { return m_extent; }

    // Framebuffer Resizing
    void resize(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D newExtent, uint32_t layers = 1);

    void cleanUp();

private:
    void createFramebuffer(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent, uint32_t layers);

    VkDevice m_device;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent{};
};