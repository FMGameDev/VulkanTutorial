#include "core/renderer/VulkanSwapChain.hpp"

#include "core/renderer/VulkanDevice.hpp"
#include "core/system/window/WindowHandler.hpp"

#include <vulkan/vk_enum_string_helper.h>
#include <stdexcept>

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
{
    SwapChainSupportDetails details;

    // This function takes the specified VkPhysicalDevice and VkSurfaceKHR window surface into account when determining the supported capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.m_capabilities);

    // Querying the supported surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        // Making sure that the vector is resized to hold all the available formats
        details.m_formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.m_formats.data());
    }

    // Querying the supported presentation modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.m_presentModes.data());
    }

    return details;
}

VulkanSwapChain::VulkanSwapChain() : m_swapChain(VK_NULL_HANDLE) {}

VulkanSwapChain::~VulkanSwapChain() {}

void VulkanSwapChain::createSwapChain(const VkDevice &device, const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface, const int widthPx, const int heightPx)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.m_formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.m_presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.m_capabilities, widthPx, heightPx);

    // How many images we would like to have in the swap chain.
    // Sticking to this minimum means that we may sometimes have to wait on the driver to complete internal operations
    // before we can acquire another image to render to.
    // Therefore it is recommended to request at least one more image than the minimum but not exceeding the maximum number of images
    // If maximum number of images is 0, it means that there is not limit
    uint32_t imageCount;
    if (swapChainSupport.m_capabilities.maxImageCount == 0 ||
        swapChainSupport.m_capabilities.minImageCount + 1 < swapChainSupport.m_capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
    }
    else
    {
        imageCount = swapChainSupport.m_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;                             // specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // If post-processing use VK_IMAGE_USAGE_TRANSFER_DST_BIT instead

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.m_graphicsFamily.value(), indices.m_presentFamily.value()};

    if (indices.m_graphicsFamily != indices.m_presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    { // most of the hardwares
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    // Transform should be applied to images in the swap chain if it is supported
    createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform; // Not specific transformation

    // Specifies if the alpha channel should be used for blending with other windows in the window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Ignore alpha channel

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; // we don't care about the color of pixels that are obscured, for example because another window is in front of them.

    createInfo.oldSwapchain = VK_NULL_HANDLE; // If the swap chain becomes invalid or unoptimized while your application is running, for example because the window was resized

    VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapChain);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("Failed to create swap chain! VkResult: ") + string_VkResult(result));
    }
    m_device = device;

    vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &supportedFormats)
{

    // Format member specifies the color channels and types
    // VK_FORMAT_B8G8R8A8_SRGB means that we store the B, G, R and alpha channels in that order with an 8 bit unsigned integer for a total of 32 bits per pixel
    // ColorSpace member indicates if the SRGB color space is supported or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag
    for (const auto &supportedFormat : supportedFormats)
    {
        if (supportedFormat.format == VK_FORMAT_B8G8R8A8_SRGB && supportedFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return supportedFormat;
        }
    }

    return supportedFormats[0];
}

// Present Mode is the most important setting for the swap chain, because it represents the actual conditions for showing images to the screen.
VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &supportedPresentModes)
{

    for (const auto &supportedPresentMode : supportedPresentModes)
    {
        // VK_PRESENT_MODE_MAILBOX_KHR is similar to VK_PRESENT_MODE_FIFO_KHR however instead of blocking the application when the queue is full,
        // the images that are already queued are simply replaced with the newer ones.
        // This mode can be used to render frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard vertical sync.
        // This is commonly known as "triple buffering".
        // On mobile devices, where energy usage is more important, VK_PRESENT_MODE_FIFO_KHR might be used instead of VK_PRESENT_MODE_MAILBOX_KHR.
        if (supportedPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return supportedPresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR; // The only present mode guaranteed to be available
}

// The swap extent is the resolution of the swap chain images
VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int widthPx, const int heightPx)
{

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent; // by default it matches the resolution of the window
    }
    else
    {
        // If the currentExtent is the maximum value of uint32_t means that the window manager allows to set special values of the width and height.
        // Then pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds
        // int width, height; // in pixels. (Vulkan only works in pixels, GLFW uses screen coordinates and pixels)

        // Query the resolution of the window in pixel before matching it against the minimum and maximum image extent.
        // m_windowHandler->GetWindowFramebufferSize(&widthPx, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(widthPx),
            static_cast<uint32_t>(heightPx)};

        // Bound the values of width and height between the allowed minimum and maximum extents that are supported by the implementation.
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanSwapChain::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];

        // Specify how the image data should be interpreted (1D textures, 2D textures, 3D textures and cube maps)
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;

        // The components field allows you to swizzle the color channels around
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // default mapping: VK_COMPONENT_SWIZZLE_IDENTITY
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // The subresourceRange field describes what the image's purpose is and which part of the image should be accessed
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(std::string("Failed to create image views! VkResult: ") + string_VkResult(result));
        }
    }
}

void VulkanSwapChain::cleanUp()
{
    for (auto imageView : m_swapChainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    m_swapChainImageViews.clear();

    if (m_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        m_swapChain = VK_NULL_HANDLE;
    }
}
