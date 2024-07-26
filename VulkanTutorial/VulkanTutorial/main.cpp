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

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow()
    {
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
            throw std::runtime_error("Failed to create GLFW window\n");
        }
        
    }
    void initVulkan() {
        
        if (!glfwVulkanSupported()) {
            throw std::runtime_error("GLFW: Vulkan Not Supported\n");
        }
        
        createInstance();

    }

    void mainLoop() {
        // Keep the application running until either an error occurs or the window is closed
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        
        vkDestroyInstance(instance, nullptr);
        
        glfwDestroyWindow(window);

        glfwTerminate();
    }
    
    void createInstance()
    {
        // struct with information about our application (optional)
        // it may provide useful information to the driver for optimization
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;
        //appInfo.pNext = nullptr;
    
        
        // Tells Vulkan driver which gloabl extentions and validation layers we want to use (mandatory)
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledLayerCount = 0;
        
        // Encountered VK_ERROR_INCOMPATIBLE_DRIVER from vkCreateInstance for MacOS with the latest MoltenVK sdk
        // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        
        std::vector<const char*> requiredExtensions;
        for(uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtensions.emplace_back(glfwExtensions[i]);
        }
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        
        // Check if all the required extensions are supported
        if(!checkGlfwExtensions(requiredExtensions, glfwExtensionCount))
        {
            throw std::runtime_error("Failed to create instance, some of the required extensions were not supported.");
        }
        
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error(std::string("Failed to create instance! VkResult: ") + string_VkResult(result));
        }
        
    }
    
    bool checkGlfwExtensions(const std::vector<const char*>& glfwRequiredExtensions, uint32_t glfwExtensionCount)
    {
        // Retrieve a list of supported extensions
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr); // Get how many extensions there are
        
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());
        
        // Print list
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
                                      if(!supported)
                                      {
                                          std::cerr << '\t' << "Required extension '" << requiredExtension << "' not supported." << '\n';
                                      }
                                      else
                                      {
                                          std::cout << '\t' << "Required extension '" << requiredExtension << "' supported." << '\n';
                                      }
                                      
                                      return supported;
                                  });
        
        return allSupported;
    }
    
    
private:
    GLFWwindow* window;
    VkInstance instance;
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

