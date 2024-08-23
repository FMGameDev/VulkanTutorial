#include "core/renderer/VulkanSurface.hpp"
#include <vulkan/vulkan_metal.h> // Include for VK_EXT_metal_surface
#include <stdexcept>

VulkanSurface::VulkanSurface() : m_surface(VK_NULL_HANDLE) {}

VulkanSurface::~VulkanSurface() {}

void VulkanSurface::createMetalSurface(VkInstance instance, void *metalLayer)
{
    m_instance = instance;

    VkMetalSurfaceCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pLayer = static_cast<const CAMetalLayer *>(metalLayer);

    // Load the function pointer for vkCreateMetalSurfaceEXT
    PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");
    if (!vkCreateMetalSurfaceEXT)
    {
        throw std::runtime_error("Unable to get pointer to function: vkCreateMetalSurfaceEXT!");
    }

    VkResult result = vkCreateMetalSurfaceEXT(instance, &createInfo, nullptr, &m_surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Metal surface!");
    }
}

void VulkanSurface::cleanUp()
{
    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}
