#define VK_USE_PLATFORM_MACOS_MVK
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_metal.h> // Include for VK_EXT_metal_surface

#include "ObjC-interface.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;     // in screen coordinates
const uint32_t HEIGHT = 600;    // in screen coordinates

//  List of required validation layers to enable
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

// List of required device extensions to enable
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined __APPLE__ && defined __arm64__
    "VK_KHR_portability_subset"
#endif
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    
    bool isAdequate() {
        return !formats.empty() && !presentModes.empty();
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
    
    // RUN FUNCTIONS //
    
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
        createSurface();
        pickPhysipcalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
    }
    
    void mainLoop() {
        // Keep the application running until either an error occurs or the window is closed
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    
    void cleanup() {
        
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }
        
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        
        vkDestroyDevice(device, nullptr);
        
        if(enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        
        vkDestroySurfaceKHR(instance, surface, nullptr);
        
        vkDestroyInstance(instance, nullptr);
        
        glfwDestroyWindow(window);
        
        glfwTerminate();
    }
    
    // END RUN FUNCTIONS //
    
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
        #if defined __APPLE__ && defined __arm64__
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        
        std::vector<const char*> requiredExtensions = getRequiredGlfwExtensions();
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
    
    // Check if all the requested layers are available
    bool checkValidationLayersSupport() {
        
        // List all the available layers
        uint32_t supportedLayerCount;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr); // Get how many layers there are
        
        std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());
        
        std::cout << "Supported validation layers: " << supportedLayers.size()  << "\n";
        for (const auto& layer : supportedLayers) {
            std::cout << '\t' << layer.layerName << '\n';
        }
        
        // Check if all the required validation layers are available
        std::cout << "Required validation layers: " << validationLayers.size()  << "\n";
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
                std::cout << '\t' << "Required validation layer '" << requestedLayerName << "' is supported." << '\n';
            }
        }
        
        std::cout << std::endl;
        
        return true;
    }
    
    std::vector<const char*> getRequiredGlfwExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        // Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
        // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        #if defined __APPLE__ && defined __arm64__
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        #endif
        
        if(enableValidationLayers) {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            // Adding KHR surface extension and MVK_MACOS_SURFACE_EXTENSION
            requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);        // necessary for creating a Vulkan surface in a platform-independent way
            requiredExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);  // modern and supported way to create a Vulkan surface on macOS, leveraging Metal for rendering.
        }
        
        // Check if all the required extensions are supported
        if(!checkGlfwExtensionsSupport(requiredExtensions))
        {
            throw std::runtime_error("Failed to create instance, some of the required extensions were not supported.");
        }
        
        return requiredExtensions;
    }
    
    bool checkGlfwExtensionsSupport(const std::vector<const char*>& glfwRequiredExtensions) {
        
        // Retrieve a list of supported extensions
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr); // Get how many extensions there are
        
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());
        
        std::cout << "Supported GLFW extensions: " << supportedExtensions.size()  << "\n";
        for (const auto& extension : supportedExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        
        // Proceed to check
        std::cout << "Required GLFW extensions: " << glfwRequiredExtensions.size()  << "\n";
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
                                                std::cerr << '\t' << "Required GLFW extension '" << requiredExtension << "' is not supported." << '\n';
                                            } else {
                                                std::cout << '\t' << "Required GLFW extension '" << requiredExtension << "' is supported." << '\n';
                                            }
                                            
                                            return supported;
                                        });
        
        std::cout << std::endl;
        
        return allSupported;
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
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
        
        return VK_FALSE;
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
    
    void createSurface() {
        
        // Retrieve the Cocoa window handle (NSWindow*)
        id windowHandle = glfwGetCocoaWindow(window);
        if(!window) {
            throw std::runtime_error(std::string("Failed to retrieve NSWindow handle!"));
        }
        
        // Get the content view and make it Metal-compatible
        id viewHandle = (id)getViewFromNSWindowPointer((void*)windowHandle);
        if(!viewHandle) {
            throw std::runtime_error(std::string("Failed to retrieve NSView handle!"));
        }
        
        VkMetalSurfaceCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pLayer = getMetalLayerFromView(viewHandle);
        
        // Load the function pointer for vkCreateMetalSurfaceEXT
        PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");
        if (!vkCreateMetalSurfaceEXT) {
            throw std::runtime_error("Unable to get pointer to function: vkCreateMetalSurfaceEXT!");
        }
        
        VkResult result = vkCreateMetalSurfaceEXT(instance, &createInfo, nullptr, &surface);
        if(result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Metal surface!");
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
        
        const bool deviceExtensionsSupported = checkDeviceExtensionSupport(device);
        if(!deviceExtensionsSupported)
        {
            return 0;
        }
        
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        if(!swapChainSupport.isAdequate())
        {
            return 0;
        }
        
        // Optional Features and Properties //
        int score = 0;
        
        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }
        else if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            score += 500;
        }
        
        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;
        
        return score;
    }
    
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t supportedDeviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedDeviceExtensionCount, nullptr);
        
        std::vector<VkExtensionProperties> supportedDeviceExtensions(supportedDeviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedDeviceExtensionCount, supportedDeviceExtensions.data());
        
        std::cout << "Supported device extension: " << supportedDeviceExtensions.size()  << "\n";
        for (const auto& extension : supportedDeviceExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        std::cout << "Required device extension: " << requiredExtensions.size()  << "\n";
        for (const auto& extension : supportedDeviceExtensions) {
            if(requiredExtensions.erase(extension.extensionName))
            {
                std::cout << '\t' << "Required device extension '" << extension.extensionName << "' is supported." << '\n';
            }
        }
        
        for (const auto& extension : requiredExtensions) {
            std::cerr << '\t' << "Required device extension '" << extension << "' is not supported." << '\n';
        }
        
        std::cout << std::endl;

        return requiredExtensions.empty();
    }
    
    void createLogicalDevice() {
        
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        // Priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0. Required
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
        createInfo.pEnabledFeatures = &deviceFeatures;
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());;
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        
        // Previous implementations of Vulkan made a distinction between instance and device specific validation layers but this is not longer the case
        // Tthe following condition will be ignored in up-to-date implementations however it has been added to make compatible with older implementations
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        
        // Instantiate the logical device
        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
        if(result != VK_SUCCESS) {
            throw std::runtime_error(std::string("Failed to create logical device! VkResult: ") + string_VkResult(result));
        }
        
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
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
            
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            
            if(indices.isComplete()) {
                break;
            }
            
            i++;
        }
        
        return indices;
    }
    
    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        
        // How many images we would like to have in the swap chain.
        // Sticking to this minimum means that we may sometimes have to wait on the driver to complete internal operations
        // before we can acquire another image to render to.
        // Therefore it is recommended to request at least one more image than the minimum but not exceeding the maximum number of images
        // If maximum number of images is 0, it means that there is not limit
        uint32_t imageCount;
        if(swapChainSupport.capabilities.maxImageCount == 0 ||
           swapChainSupport.capabilities.minImageCount + 1 < swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.minImageCount + 1;
        } else {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // If post-processing use VK_IMAGE_USAGE_TRANSFER_DST_BIT instead
        
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        if(indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else { // most of the hardwares
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;       // Optional
            createInfo.pQueueFamilyIndices = nullptr;   // Optional
        }
        
        // Transform should be applied to images in the swap chain if it is supported
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform; // Not specific transformation
        
        // Specifies if the alpha channel should be used for blending with other windows in the window system
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Ignore alpha channel
        
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE; // we don't care about the color of pixels that are obscured, for example because another window is in front of them.
        
        createInfo.oldSwapchain = VK_NULL_HANDLE; // If the swap chain becomes invalid or unoptimized while your application is running, for example because the window was resized
        
        VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
        if(result != VK_SUCCESS) {
            throw std::runtime_error(std::string("Failed to create swap chain! VkResult: ") + string_VkResult(result));
        }
        
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }
    
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        
        // This function takes the specified VkPhysicalDevice and VkSurfaceKHR window surface into account when determining the supported capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        
        // Querying the supported surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if(formatCount != 0) {
            // Making sure that the vector is resized to hold all the available formats
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }
        
        // Querying the supported presentation modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if(presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        
        return details;
    }
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& supportedFormats) {
        
        // Format member specifies the color channels and types
        // VK_FORMAT_B8G8R8A8_SRGB means that we store the B, G, R and alpha channels in that order with an 8 bit unsigned integer for a total of 32 bits per pixel
        // ColorSpace member indicates if the SRGB color space is supported or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag
        for( const auto& supportedFormat : supportedFormats) {
            if(supportedFormat.format == VK_FORMAT_B8G8R8A8_SRGB && supportedFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return supportedFormat;
            }
        }
        
        return supportedFormats[0];
    }
    
    // Present Mode is the most important setting for the swap chain, because it represents the actual conditions for showing images to the screen.
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& supportedPresentModes) {
        
        for (const auto& supportedPresentMode : supportedPresentModes) {
            // VK_PRESENT_MODE_MAILBOX_KHR is similar to VK_PRESENT_MODE_FIFO_KHR however instead of blocking the application when the queue is full,
            // the images that are already queued are simply replaced with the newer ones.
            // This mode can be used to render frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard vertical sync.
            // This is commonly known as "triple buffering".
            // On mobile devices, where energy usage is more important, VK_PRESENT_MODE_FIFO_KHR might be used instead of VK_PRESENT_MODE_MAILBOX_KHR.
            if (supportedPresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return supportedPresentMode;
            }
        }
        
        return VK_PRESENT_MODE_FIFO_KHR; // The only present mode guaranteed to be available
    }
    
    // The swap extent is the resolution of the swap chain images
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        
        if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent; // by default it matches the resolution of the window
        } else {
            // If the currentExtent is the maximum value of uint32_t means that the window manager allows to set special values of the width and height.
            // Then pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds
            int width, height; // in pixels. (Vulkan only works in pixels, GLFW uses screen coordinates and pixels)
            
            // Query the resolution of the window in pixel before matching it against the minimum and maximum image extent.
            glfwGetFramebufferSize(window, &width, &height);
            
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
            
            // Bound the values of width and height between the allowed minimum and maximum extents that are supported by the implementation.
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            
            return actualExtent;
        }
    }
    
    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());
        
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            
            // Specify how the image data should be interpreted (1D textures, 2D textures, 3D textures and cube maps)
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            
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
            
            VkResult result = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
            if(result != VK_SUCCESS) {
                throw std::runtime_error(std::string("Failed to create image views! VkResult: ") + string_VkResult(result));
            }
            
        }
    }
    
private:
    GLFWwindow* window;
    
    VkInstance instance;
    
    VkDebugUtilsMessengerEXT debugMessenger;
    
    VkSurfaceKHR surface; // window surface creation
    
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // implicitly destroyed when the VkInstance is destroyed
    
    VkDevice device; // logical device
    // Queue Family
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    
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

