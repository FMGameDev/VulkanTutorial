#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanValidationLayer
{
public:
    VulkanValidationLayer();
    ~VulkanValidationLayer();

    const std::vector<const char *> &getValidationLayers() const { return m_validationLayers; }
    const bool isValidationLayersEnabled() const { return m_enableValidationLayers; }

    bool checkValidationLayersSupport() const;

private:
    const std::vector<const char *> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation" // Common validation layer for Vulkan
    };

#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif
};
