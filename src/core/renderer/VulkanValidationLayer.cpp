#include "core/renderer/VulkanValidationLayer.hpp"

#include <iostream>

VulkanValidationLayer::VulkanValidationLayer() {}

VulkanValidationLayer::~VulkanValidationLayer() {}

// Check if all the requested layers are available
bool VulkanValidationLayer::checkValidationLayersSupport() const
{
    // List all the available layers
    uint32_t supportedLayerCount;
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr); // Get how many layers there are

    std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

    std::cout << "Supported validation layers: " << supportedLayers.size() << "\n";
    for (const auto &layer : supportedLayers)
    {
        std::cout << '\t' << layer.layerName << '\n';
    }

    // Check if all the required validation layers are available
    std::cout << "Required validation layers: " << m_validationLayers.size() << "\n";
    for (const char *requestedLayerName : m_validationLayers)
    {
        bool layerFound = false;

        for (const auto &suportedLayer : supportedLayers)
        {
            if (strcmp(requestedLayerName, suportedLayer.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
        {
            std::cerr << '\t' << "Required validation layer '" << requestedLayerName << "' is not supported." << '\n';
            return false;
        }
        else
        {
            std::cout << '\t' << "Required validation layer '" << requestedLayerName << "' is supported." << '\n';
        }
    }

    std::cout << std::endl;

    return true;
}
