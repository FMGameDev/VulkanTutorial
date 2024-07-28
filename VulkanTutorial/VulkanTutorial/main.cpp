#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = { 
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}


class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        
        // Initializes the GLFW library
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW\n");
        }
        
        // No OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Don't allow to resize window
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        // Create window
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        if(!window)
        {
            throw std::runtime_error("Failed to create GLFW Window\n");
        }
    }
    
    void initVulkan() {
        
        if (!glfwVulkanSupported()) {
            throw std::runtime_error("GLFW: Vulkan Not Supported\n");
        }
        createInstance();
        setupDebugMessenger();
        pickPhysipcalDevice();
    }

    void mainLoop() {
        
        // Keep the application running until either an error occurs or the window is closed
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        
        if(enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        
        vkDestroyInstance(instance, nullptr);
        
        glfwDestroyWindow(window);

        glfwTerminate();
    }
    
    void createInstance()
    {
        if(enableValidationLayers && !checkValidationLayersSupport()) {
            throw std::runtime_error("Some of the validation layers requested were not available!");
        }
        
        // struct with information about our application (optional)
        // it may provide useful information to the driver for optimization
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Tells Vulkan driver which global extentions and validation layers we want to use (mandatory)
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
 
        // Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
        // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        
        std::vector<const char*> requiredExtensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            
            createInfo.pNext = nullptr;
        }
        
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error(std::string("Failed to create instance! VkResult: ") + string_VkResult(result));
        }
    }
    
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }
    
    void setupDebugMessenger() {
        if(!enableValidationLayers)
            return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        
        VkResult result = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error(std::string("Failed to set up debug messenger! VkResult: ") + string_VkResult(result));
        }
    }
    
    void pickPhysipcalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if(deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;
        
        for(const auto& device : devices) {
            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }
        
        // Check if the best candidate is suitable
        if(candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }
    
    int rateDeviceSuitability(VkPhysicalDevice device) {
    
        // Get basic device properties like the name, type and supported Vulkan version
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        
        // Get Support for optional features like texture compresion, 64 bit floats and multi viewport rendering(useful for VR)
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        // Mandatory Features and Properties //
        //if (!deviceFeatures.geometryShader) { // Note: Vulkan on Mac (MoltenVK) does not support Geometry Shader, it will return false even if this device supports geometry shaders
        //    return 0;
        //}
        QueueFamilyIndices indices = findQueueFamilies(device);
        if(!indices.isComplete())
        {
            return 0;
        }
        
        // Optional Features and Properties //
        int score = 0;
        
        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }
        else if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            score += 500;
        }
        
        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;
        
        return score;
    }
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        // Logic to find queue family to populate struct with
        QueueFamilyIndices indices;
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        
        int i = 0;
        for(const auto& queueFamily : queueFamilies) {
            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            
            if(indices.isComplete()) {
                break;
            }
            
            i++;
        }
        
        return indices;
    }
    
    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
        // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        if(enableValidationLayers) {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        
        // Check if all the required extensions are supported
        if(!checkGlfwExtensionsSupport(requiredExtensions))
        {
            throw std::runtime_error("Failed to create instance, some of the required extensions were not supported.");
        }

        return requiredExtensions;
    }
    
    // Check if all the requested layers are available
    bool checkValidationLayersSupport() {
        
        // List all the available layers
        uint32_t supportedLayerCount;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr); // Get how many layers there are
        
        std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());
        
        std::cout << "Available layers:" << supportedLayers.size()  << "\n";
        for (const auto& layer : supportedLayers) {
            std::cout << '\t' << layer.layerName << '\n';
        }
        
        // Check if all the required validation layers are available
        std::cout << "Required validation layers:" << validationLayers.size()  << "\n";
        for (const char* requestedLayerName : validationLayers) {
            bool layerFound = false;
            
            for (const auto& suportedLayer : supportedLayers) {
                if (strcmp(requestedLayerName, suportedLayer.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if(!layerFound) {
                std::cerr << '\t' << "Required validation layer '" << requestedLayerName << "' is not supported." << '\n';
                return false;
            } else {
                std::cerr << '\t' << "Required validation layer '" << requestedLayerName << "' is supported." << '\n';
            }
        }
        
        return true;
    }
    
    bool checkGlfwExtensionsSupport(const std::vector<const char*>& glfwRequiredExtensions) {
        
        // Retrieve a list of supported extensions
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr); // Get how many extensions there are
        
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());
        
        std::cout << "Available extensions:" << supportedExtensions.size()  << "\n";
        for (const auto& extension : supportedExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        
        // Proceed to check
        std::cout << "Required extensions:" << glfwRequiredExtensions.size()  << "\n";
        bool allSupported = std::all_of(
                                  glfwRequiredExtensions.begin(),
                                  glfwRequiredExtensions.end(),
                                  [&supportedExtensions](const char* requiredExtension) {
                                     bool supported = std::any_of(
                                                         supportedExtensions.begin(),
                                                         supportedExtensions.end(),
                                                         [requiredExtension](const VkExtensionProperties& ext) {
                                                             return strcmp(ext.extensionName, requiredExtension) == 0;
                                                         });
                                      if(!supported) {
                                          std::cerr << '\t' << "Required extension '" << requiredExtension << "' is not supported." << '\n';
                                      } else {
                                          std::cout << '\t' << "Required extension '" << requiredExtension << "' is supported." << '\n';
                                      }
                                      
                                      return supported;
                                  });
        
        return allSupported;
    }
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    
    
private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // implicitly destroyed when the VkInstance is destroyed
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

