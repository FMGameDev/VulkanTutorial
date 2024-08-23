#pragma once

#include <vulkan/vulkan.h>

class VulkanSurface
{
public:
    VulkanSurface();
    ~VulkanSurface();

    void createMetalSurface(VkInstance instance, void *metalLayer);
    void cleanUp();

    VkSurfaceKHR getSurface() const { return m_surface; }

private:
    VkInstance m_instance;
    VkSurfaceKHR m_surface;
};
