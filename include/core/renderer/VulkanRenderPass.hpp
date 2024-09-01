#include <vulkan/vulkan.h>
#include <stdexcept>

class VulkanRenderPass
{
public:
    VulkanRenderPass(const VkDevice &device);
    ~VulkanRenderPass();

    void createRenderPass(
        VkFormat colorFormat,
        VkFormat depthFormat = VK_FORMAT_UNDEFINED,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

    VkRenderPass getRenderPass() const { return m_renderPass; }
    void cleanUp();

private:
    VkDevice m_device;
    VkRenderPass m_renderPass;

    VkAttachmentDescription createColorAttachment(VkFormat format, VkSampleCountFlagBits samples) const;
    VkAttachmentDescription createDepthAttachment(VkFormat format, VkSampleCountFlagBits samples) const;
};