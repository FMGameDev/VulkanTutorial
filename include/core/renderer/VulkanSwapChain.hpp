#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class WindowHandler;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_presentModes;

    bool isAdequate()
    {
        return !m_formats.empty() && !m_presentModes.empty();
    }
};

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

class VulkanSwapChain
{
public:
    VulkanSwapChain();
    ~VulkanSwapChain();

    void createSwapChain(const VkDevice &device, const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface, const int widthPx, const int heightPx);
    void createImageViews();

    void cleanUp();

    VkSwapchainKHR getSwapChain() const { return m_swapChain; }
    VkExtent2D getSwapChainExtent() const { return m_swapChainExtent; }
    VkFormat getSwapChainFormat() const { return m_swapChainImageFormat; }
    std::vector<VkImageView> getSwapChainImageViews() const { return m_swapChainImageViews; }

private:
    VkSwapchainKHR m_swapChain;
    VkDevice m_device;

    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &supportedFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &supportedPresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int widthPx, const int heightPx);
};
